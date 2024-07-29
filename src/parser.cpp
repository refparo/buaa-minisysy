#include "parser.hpp"

using namespace ast;

static Global parse_global(Lexer & lexer);

static Block parse_block(Lexer & lexer);

static Stmt parse_stmt(Lexer & lexer);
static Stmt parse_stmt_without_semicolon(Lexer & lexer);

static VarDecl parse_var_decl(Lexer & lexer, bool is_const);
static VarDecl parse_var_decl(Lexer & lexer, bool is_const, Type type);
static VarDecl parse_var_decl(Lexer & lexer, bool is_const, Type type, Ident && first_name);

static Expr parse_expr(Lexer & lexer, int prec = 0);
static Expr parse_expr_beginning_with_ident(Lexer & lexer, Ident && ident, int prec = 0);
static Expr parse_unary_expr(Lexer & lexer);
static Expr parse_core_expr(Lexer & lexer);
static Expr parse_var_or_func_call(Lexer & lexer, Ident && ident);

static Type parse_type(Lexer & lexer);

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)

Program parse(Lexer & lexer) {
  Program program;
  while (lexer.peek().tag != Token::ERR) {
    program.push_back(parse_global(lexer));
  }
  return program;
}

Global parse_global(Lexer & lexer) {
  switch (lexer.peek().tag) {
  case Token::CONST:
  {
    lexer.get();
    VarDecl result = parse_var_decl(lexer, true);
    if (lexer.get().tag != Token::SEMICOLON) throw "expected ';'";
    return result;
  }
  default:
    Type type = parse_type(lexer);
    Ident name = lexer.get().ident;
    if (type == Type::VOID) {
      if (lexer.get().tag != Token::LPAR) throw "expected '('";
    } else if (lexer.peek().tag == Token::LPAR) {
      lexer.get();
    } else { // is var
      VarDecl result = parse_var_decl(lexer, false, type, std::move(name));
      if (lexer.get().tag != Token::SEMICOLON) throw "expected ';'";
      return result;
    }
    std::vector<ArgDef> args;
    if (lexer.peek().tag != Token::RPAR) {
      while (true) {
        Type type = parse_type(lexer);
        if (lexer.peek().tag != Token::IDENT) throw "expected identifier";
        args.push_back(ArgDef{type, lexer.get().ident});
        if (lexer.peek().tag != Token::COMMA) break;
        lexer.get();
      }
      if (lexer.peek().tag != Token::RPAR) throw "expected ')'";
    }
    lexer.get();
    return Func{type, std::move(name), args, parse_block(lexer)};
  }
}

Block parse_block(Lexer & lexer) {
  Token tok = lexer.get();
  if (tok.tag != Token::LBRACE) throw "expected '{'";
  Block body;
  while (lexer.peek().tag != Token::RBRACE) {
    body.push_back(parse_stmt(lexer));
  }
  tok = lexer.get();
  return body;
}

Stmt parse_stmt(Lexer & lexer) {
  switch (lexer.peek().tag) {
  case Token::SEMICOLON:
    lexer.get();
    return std::monostate{};
  case Token::IF:
  {
    lexer.get();
    if (lexer.get().tag != Token::LPAR) throw "expected '('";
    Expr cond = parse_expr(lexer);
    if (lexer.get().tag != Token::RPAR) throw "expected ')'";
    Stmt true_body = parse_stmt(lexer);
    if (lexer.peek().tag == Token::ELSE) {
      lexer.get();
      Stmt false_body = parse_stmt(lexer);
      return IfElse{
        std::move(cond),
        std::make_unique<Stmt>(std::move(true_body)),
        std::make_unique<Stmt>(std::move(false_body))
      };
    } else {
      return If{
        std::move(cond),
        std::make_unique<Stmt>(std::move(true_body))
      };
    }
  }
  case Token::WHILE:
  {
    lexer.get();
    if (lexer.get().tag != Token::LPAR) throw "expected '('";
    Expr cond = parse_expr(lexer);
    if (lexer.get().tag != Token::RPAR) throw "expected ')'";
    Stmt body = parse_stmt(lexer);
    return While{
      std::move(cond),
      std::make_unique<Stmt>(std::move(body))
    };
  }
  case Token::LBRACE:
    return parse_block(lexer);
  default:
    Stmt stmt = parse_stmt_without_semicolon(lexer);
    if (lexer.get().tag != Token::SEMICOLON) {
      throw "expected ';'";
    }
    return stmt;
  }
}

Stmt parse_stmt_without_semicolon(Lexer & lexer) {
  switch (lexer.peek().tag) {
  case Token::RETURN: // return
    lexer.get();
    if (lexer.peek().tag == Token::SEMICOLON) return Return{{}};
    else return Return{parse_expr(lexer)};
  case Token::BREAK:
    lexer.get();
    return Break{};
  case Token::CONTINUE:
    lexer.get();
    return Continue{};
  case Token::IDENT: // assign or expr
  {
    Ident ident = lexer.get().ident;
    switch (lexer.peek().tag) {
      case Token::ASSIGN: //assign
        lexer.get();
        return Assign{std::move(ident), parse_expr(lexer)};
      default: // expr
        return parse_expr_beginning_with_ident(lexer, std::move(ident));
    }
  }
  case Token::CONST: // decl
    lexer.get();
    return parse_var_decl(lexer, true);
  case Token::INT:
    return parse_var_decl(lexer, false);
  default: // expr
    return parse_expr(lexer);
  }
}

VarDecl parse_var_decl(Lexer & lexer, bool is_const) {
  return parse_var_decl(lexer, is_const, parse_type(lexer));
}

VarDecl parse_var_decl(Lexer & lexer, bool is_const, Type type) {
  if (lexer.peek().tag != Token::IDENT) throw "expected identifier";
  return parse_var_decl(lexer, is_const, type, lexer.get().ident);
}

VarDecl parse_var_decl(Lexer & lexer, bool is_const, Type type, Ident && name) {
  std::vector<VarDef> defs;
  while (true) {
    if (lexer.peek().tag == Token::ASSIGN) {
      lexer.get();
      defs.push_back(VarDef{name, parse_expr(lexer)});
    } else {
      if (is_const) throw "expected '='";
      defs.push_back(VarDef{name, {}});
    }
    if (lexer.peek().tag != Token::COMMA) break;
    lexer.get();
    if (lexer.peek().tag != Token::IDENT) throw "expected identifier";
    name = lexer.get().ident;
  }
  return VarDecl{is_const, type, std::move(defs)};
}

// for binary op, returns precedence; for others, returns 0
static constexpr int prec(Token::Tag op) {
  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
  switch (op) {
  case Token::MULT:
  case Token::DIV:
  case Token::MOD:
    return 9;
  case Token::PLUS:
  case Token::MINUS:
    return 8;
  case Token::LT:
  case Token::LTEQ:
  case Token::GT:
  case Token::GTEQ:
    return 7;
  case Token::EQ:
  case Token::NEQ:
    return 6;
  case Token::AND:
    return 5;
  case Token::OR:
    return 4;
  default:
    return 0;
  }
  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
}

static constexpr Binary::Op tok_to_binary(Token::Tag op) {
  switch (op) {
  case Token::DIV: return Binary::DIV;
  case Token::MOD: return Binary::MOD;
  case Token::PLUS: return Binary::PLUS;
  case Token::MINUS: return Binary::MINUS;
  case Token::MULT: return Binary::MULT;
  case Token::LT: return Binary::LT;
  case Token::LTEQ: return Binary::LTEQ;
  case Token::GT: return Binary::GT;
  case Token::GTEQ: return Binary::GTEQ;
  case Token::EQ: return Binary::EQ;
  case Token::NEQ: return Binary::NEQ;
  case Token::AND: return Binary::AND;
  case Token::OR: return Binary::OR;
  default: throw "not a binary operator!";
  }
}

Expr parse_expr(Lexer & lexer, int prev_prec) {
  Expr lhs = parse_unary_expr(lexer);
  while (prec(lexer.peek().tag) > prev_prec) {
    Token tok = lexer.get();
    Expr rhs = parse_expr(lexer, prec(tok.tag));
    lhs = Binary{
      tok_to_binary(tok.tag),
      std::make_unique<Expr>(std::move(lhs)),
      std::make_unique<Expr>(std::move(rhs))
    };
  }
  return lhs;
}

Expr parse_expr_beginning_with_ident(Lexer & lexer, Ident && ident, int prev_prec) {
  Expr lhs = parse_var_or_func_call(lexer, std::move(ident));
  while (prec(lexer.peek().tag) > prev_prec) {
    Token tok = lexer.get();
    Expr rhs = parse_expr(lexer, prec(tok.tag));
    lhs = Binary{
      tok_to_binary(tok.tag),
      std::make_unique<Expr>(std::move(lhs)),
      std::make_unique<Expr>(std::move(rhs))
    };
  }
  return lhs;
}

static constexpr bool is_unary(Token::Tag op) {
  switch (op) {
  case Token::PLUS:
  case Token::MINUS:
  case Token::NOT:
    return true;
  default:
    return false;
  }
}

static constexpr Unary::Op tok_to_unary(Token::Tag op) {
  switch (op) {
  case Token::PLUS: return Unary::POS;
  case Token::MINUS: return Unary::NEG;
  case Token::NOT: return Unary::NOT;
  default: throw "not a unary operator!";
  }
}

Expr parse_unary_expr(Lexer & lexer) {
  std::vector<Token> stack;
  while (is_unary(lexer.peek().tag)) {
    stack.push_back(lexer.get());
  }
  Expr expr = parse_core_expr(lexer);
  while (!stack.empty()) {
    expr = Unary{
      tok_to_unary(stack.back().tag),
      std::make_unique<Expr>(std::move(expr))
    };
    stack.pop_back();
  }
  return expr;
}

Expr parse_core_expr(Lexer & lexer) {
  Token tok = lexer.get();
  switch (tok.tag) {
  case Token::NUMBER:
    return tok.number;
  case Token::LPAR:
  {
    Expr inner = parse_expr(lexer);
    tok = lexer.get();
    if (tok.tag == Token::RPAR) {
      return inner;
    } else {
      throw "expected ')'";
    }
  }
  case Token::IDENT:
    return parse_var_or_func_call(lexer, std::move(tok.ident));
  default:
    throw "expected expr";
  }
}

Expr parse_var_or_func_call(Lexer & lexer, Ident && ident) {
  if (lexer.peek().tag == Token::LPAR) {
    lexer.get();
    if (lexer.peek().tag == Token::RPAR) {
      lexer.get();
      return FuncCall{ident, {}};
    } else {
      std::vector<Expr> args;
      while (true) {
        args.push_back(parse_expr(lexer));
        if (lexer.peek().tag != Token::COMMA) break;
        lexer.get();
      }
      if (lexer.get().tag != Token::RPAR) throw "expected ')'";
      return FuncCall{ident, std::move(args)};
    }
  } else {
    return ident;
  }
}

Type parse_type(Lexer & lexer) {
  switch (lexer.get().tag) {
  case Token::INT: return Type::INT;
  case Token::VOID: return Type::VOID;
  default: throw "expected 'int' or 'void'";
  }
}

// NOLINTEND(cppcoreguidelines-pro-type-union-access)
