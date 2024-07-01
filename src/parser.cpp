#include <memory>
#include <vector>

#include "parser.hpp"

using namespace ast;

Program Parser::parse() {
  return Program{parse_func()};
}

Func Parser::parse_func() {
  // rettype
  Token tok = lexer.get();
  if (tok.tag != Token::IDENT) throw "expected identifier";
  Ident rettype = std::move(tok.ident);
  // name
  tok = lexer.get();
  if (tok.tag != Token::IDENT) throw "expected identifier";
  Ident name = std::move(tok.ident);
  // ()
  tok = lexer.get();
  if (tok.tag != Token::LPAR) throw "expected '('";
  tok = lexer.get();
  if (tok.tag != Token::RPAR) throw "expected ')'";
  // body
  Block body = parse_block();
  return Func{rettype, name, std::move(body)};
}

Block Parser::parse_block() {
  // {
  Token tok = lexer.get();
  if (tok.tag != Token::LBRACE) throw "expected '{'";
  // stmt
  Stmt stmt = parse_stmt();
  // }
  tok = lexer.get();
  if (tok.tag != Token::RBRACE) throw "expected '}'";
  return Block{std::move(stmt)};
}

Stmt Parser::parse_stmt() {
  // return
  Token tok = lexer.get();
  if (tok.tag != Token::RETURN) throw "expected 'return'";
  // expr
  Expr retval = parse_expr();
  // ;
  tok = lexer.get();
  if (tok.tag != Token::SEMICOLON) throw "expected ';'";
  return Return{std::move(retval)};
}

// for binary op, returns precedence; for others, returns 0
static constexpr int prec(Token::Tag op) {
  switch (op) {
  case Token::PLUS: return 1;
  case Token::MINUS: return 1;
  case Token::MULT: return 2;
  case Token::DIV: return 2;
  case Token::MOD: return 2;
  default: return 0;
  }
}

static constexpr Binary::Op tok_to_binary(Token::Tag op) {
  switch (op) {
  case Token::PLUS: return Binary::PLUS;
  case Token::MINUS: return Binary::MINUS;
  case Token::MULT: return Binary::MULT;
  case Token::DIV: return Binary::DIV;
  case Token::MOD: return Binary::MOD;
  default: throw "not a binary operator!";
  }
}

Expr Parser::parse_expr(int prev_prec) {
  Expr lhs = parse_unary_expr();
  while (prec(lexer.peek().tag) > prev_prec) {
    Token tok = lexer.get();
    Expr rhs = parse_expr(prec(tok.tag));
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
  case Token::PLUS: return true;
  case Token::MINUS: return true;
  default: return false;
  }
}
static constexpr Unary::Op tok_to_unary(Token::Tag op) {
  switch (op) {
  case Token::PLUS: return Unary::POS;
  case Token::MINUS: return Unary::NEG;
  default: throw "not a unary operator!";
  }
}

Expr Parser::parse_unary_expr() {
  std::vector<Token> stack;
  while (is_unary(lexer.peek().tag)) {
    stack.push_back(lexer.get());
  }
  Expr expr = parse_par_expr_or_number();
  while (!stack.empty()) {
    expr = Unary{
      tok_to_unary(stack.back().tag),
      std::make_unique<Expr>(std::move(expr))
    };
    stack.pop_back();
  }
  return expr;
}

Expr Parser::parse_par_expr_or_number() {
  Token tok = lexer.get();
  if (tok.tag == Token::NUMBER) {
    return tok.number;
  }
  if (tok.tag == Token::LPAR) {
    Expr inner = parse_expr();
    tok = lexer.get();
    if (tok.tag == Token::RPAR) {
      return inner;
    } else {
      throw "expected ')'";
    }
  }
  throw "expected expr";
}
