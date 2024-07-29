#include <iterator>
#include <ranges>

#include "codegen.hpp"
#include "ir.hpp"
#include "overloaded.hpp"

static ir::Type ast_type_to_ir_type(ast::Type type) {
  switch (type) {
  case ast::INT: return ir::I32;
  case ast::VOID: return ir::VOID;
  }
}

// initialize global context
Codegen::Codegen() : scopes{Scope{}} {}

ir::Program Codegen::get() && {
  ir::Program result;
  if (this->scopes.front().contains("getch")) {
    result.emplace_back(ir::FuncDecl{ir::I32, "getch", {}});
  }
  if (this->scopes.front().contains("putch")) {
    result.emplace_back(ir::FuncDecl{ir::VOID, "putch", {ir::I32}});
  }
  if (this->scopes.front().contains("getint")) {
    result.emplace_back(ir::FuncDecl{ir::I32, "getint", {}});
  }
  if (this->scopes.front().contains("putint")) {
    result.emplace_back(ir::FuncDecl{ir::VOID, "putint", {ir::I32}});
  }
  result.insert(
    result.end(),
    std::make_move_iterator(this->ir.begin()),
    std::make_move_iterator(this->ir.end())
  );
  this->ir.clear();
  return std::move(result);
}

void Codegen::add_program(const ast::Program & program) {
  for (auto & def : program) {
    std::visit(overloaded {
      [this](const ast::Func & func) {
        add_func(func);
      },
      [this](const ast::VarDecl & decl) {
        add_var_decl(decl);
      }
    }, def);
  }
}

void Codegen::add_func(const ast::Func & func) {
  Scope scope;
  std::vector<ir::Type> args;

  // args
  for (int i = 0; i < func.args.size(); i++) {
    auto & arg = func.args[i];
    if (arg.type != ast::INT) throw "unsupported argument type";
    auto [_, success] = scope.insert({
      arg.name,
      // we can't assign to arguments!
      Symbol{Symbol::CONST, ir::I32, 0, ir::Arg{i}}
    });
    if (!success) throw "duplicate argument name";
    args.push_back(ir::I32);
  }

  // add func to ir
  ir::Type rettype = ast_type_to_ir_type(func.rettype);
  this->ir.emplace_back(ir::Func{
    rettype,
    func.name,
    args,
    {ir::Block{}}
  });

  // add func to scope
  auto [_, success] = get_scope().insert({
    func.name,
    Symbol{Symbol::FUNC, rettype, int(args.size()), ir::Global{func.name}}
  });
  if (!success) throw "duplicate function name";

  // push function scope
  this->scopes.push_back(std::move(scope));

  // add the function body
  for (auto & stmt : func.body) {
    add_stmt(stmt);
  }

  // remove empty block at the end of function
  if (get_func().blocks.back().empty()) {
    get_func().blocks.pop_back();
  }

  // pop function scope
  this->scopes.pop_back();
}

void Codegen::add_var_decl(const ast::VarDecl & decl) {
  switch (decl.type) {
  case ast::Type::VOID:
    throw "variables can't be void";
  case ast::Type::INT:
    if (decl.is_const) {
      for (auto & def : decl.defs) {
        if (!def.init.has_value()) throw "constant must be initialized";
        auto [_, success] = get_scope().insert({def.name,
          Symbol{
            Symbol::CONST,
            ir::I32,
            0,
            ir::Const{eval_constexpr(def.init.value())}
          }
        });
        if (!success) throw "redeclared constant";
      }
    } else {
      if (this->scopes.size() == 1) {
        // if global
        for (auto & def : decl.defs) {
          this->ir.emplace_back(ir::GlobalVar{
            def.name,
            ir::I32,
            def.init.has_value()
            ? eval_constexpr(def.init.value())
            : 0
          });
          auto [_, success] = get_scope().insert(
            {def.name, Symbol{Symbol::VAR, ir::I32, 0, ir::Global{def.name}}}
          );
          if (!success) throw "redeclared variable";
        }
      } else {
        // if in block
        auto block = get_block();
        for (auto & def : decl.defs) {
          auto var = block->push_back(ir::Alloca{ir::I32});
          auto [_, success] = get_scope().insert(
            {def.name, Symbol{Symbol::VAR, ir::I32, 0, var}}
          );
          if (!success) throw "redeclared variable";
          if (def.init.has_value()) {
            auto result = cast(add_expr(def.init.value()), ir::I32);
            block->push_back(ir::Store{ir::I32, result, var});
          }
        }
      }
    }
  }
}

void Codegen::add_stmt(const ast::Stmt & stmt) {
  std::visit(overloaded {
    [this](std::monostate _) { /* noop */ },
    [this](const ast::If & stmt) {
      auto & func = get_func();

      // cond block
      auto cond = cast(add_expr(stmt.cond), ir::I1);
      auto cond_end = --func.blocks.end();

      // body block
      auto body_begin = func.new_block();
      add_stmt(*stmt.true_body);
      auto body_end = --func.blocks.end();

      // after block
      auto after_if = func.new_block();

      // add branches
      cond_end->terminator = ir::BrCond{cond, body_begin, after_if};
      body_end->terminator = ir::Br{after_if};
    },
    [this](const ast::IfElse & stmt) {
      auto & func = get_func();

      // cond block
      auto cond = cast(add_expr(stmt.cond), ir::I1);
      auto cond_end = --func.blocks.end();

      // true block
      auto true_begin = func.new_block();
      add_stmt(*stmt.true_body);
      auto true_end = --func.blocks.end();

      // false block
      auto false_begin = func.new_block();
      add_stmt(*stmt.false_body);
      auto false_end = --func.blocks.end();

      // after block
      auto after_if = func.new_block();

      // add branches
      cond_end->terminator = ir::BrCond{cond, true_begin, false_begin};
      true_end->terminator = ir::Br{after_if};
      false_end->terminator = ir::Br{after_if};
    },
    [this](const ast::While & stmt) {
      auto & func = get_func();

      // before block
      auto before_loop = --func.blocks.end();

      // cond block
      auto cond_begin = func.new_block();
      auto cond = cast(add_expr(stmt.cond), ir::I1);
      auto cond_end = --func.blocks.end();

      // body block
      auto body_begin = func.new_block();
      this->loop_contexts.push_back(LoopContext{cond_begin});
      add_stmt(*stmt.body);
      auto body_end = --func.blocks.end();

      // after block
      auto after_loop = func.new_block();

      // add conditional branch and fix breaks
      before_loop->terminator = ir::Br{cond_begin};
      cond_end->terminator = ir::BrCond{cond, body_begin, after_loop};
      for (auto break_block : this->loop_contexts.back().breaks) {
        break_block->terminator = ir::Br{after_loop};
      }
      this->loop_contexts.pop_back();
      body_end->terminator = ir::Br{cond_begin};
    },
    [this](const ast::Block & block) {
      this->scopes.emplace_back();
      for (auto & stmt : block) {
        add_stmt(stmt);
      }
      this->scopes.pop_back();
    },
    [this](const ast::Assign & stmt) {
      auto & symbol = get_symbol(stmt.var);
      if (symbol.kind != Symbol::VAR) throw "can't assign to constant or function";
      get_block()->push_back(ir::Store{
        symbol.type,
        cast(add_expr(stmt.value), symbol.type),
        symbol.ir
      });
    },
    [this](const ast::Return & stmt) {
      if (stmt.retval.has_value()) {
        if (get_func().rettype != ir::I32) {
          throw "can't return a value from a function with rettype void";
        }
        get_block()->terminator = ir::Ret{
          ir::I32,
          cast(add_expr(stmt.retval.value()), ir::I32)
        };
      } else {
        if (get_func().rettype != ir::VOID) {
          throw "can't return without a value from a function with rettype int";
        }
        get_block()->terminator = ir::Ret{ir::VOID};
      }
      get_func().new_block();
    },
    [this](const ast::Break & _) {
      auto & context = get_loop_context();
      context.breaks.push_back(get_block());
      get_func().new_block();
    },
    [this](const ast::Continue & _) {
      auto & context = get_loop_context();
      get_block()->terminator = ir::Br{context.loop_begin};
      get_func().new_block();
    },
    [this](const ast::Expr & expr) {
      add_expr(expr);
    },
    [this](const ast::VarDecl & decl) {
      add_var_decl(decl);
    }
  }, stmt);
}

TypedOperand Codegen::add_expr(const ast::Expr & expr) {
  return std::visit(overloaded {
    [this](const ast::Binary & expr) {
      // NOLINTBEGIN(cppcoreguidelines-init-variables)
      ir::Binary::Op op;
      ir::Type operand_type;
      ir::Type result_type;
      // NOLINTEND(cppcoreguidelines-init-variables)
      switch (expr.op) {
      case ast::Binary::PLUS:
        op = ir::Binary::ADD;
        operand_type = ir::I32;
        result_type = ir::I32;
        break;
      case ast::Binary::MINUS:
        op = ir::Binary::SUB;
        operand_type = ir::I32;
        result_type = ir::I32;
        break;
      case ast::Binary::MULT:
        op = ir::Binary::MUL;
        operand_type = ir::I32;
        result_type = ir::I32;
        break;
      case ast::Binary::DIV:
        op = ir::Binary::SDIV;
        operand_type = ir::I32;
        result_type = ir::I32;
        break;
      case ast::Binary::MOD:
        op = ir::Binary::SREM;
        operand_type = ir::I32;
        result_type = ir::I32;
        break;
      case ast::Binary::LT:
        op = ir::Binary::ICMP_SLT;
        operand_type = ir::I32;
        result_type = ir::I1;
        break;
      case ast::Binary::LTEQ:
        op = ir::Binary::ICMP_SLE;
        operand_type = ir::I32;
        result_type = ir::I1;
        break;
      case ast::Binary::GT:
        op = ir::Binary::ICMP_SGT;
        operand_type = ir::I32;
        result_type = ir::I1;
        break;
      case ast::Binary::GTEQ:
        op = ir::Binary::ICMP_SGE;
        operand_type = ir::I32;
        result_type = ir::I1;
        break;
      case ast::Binary::EQ:
        op = ir::Binary::ICMP_EQ;
        operand_type = ir::I32;
        result_type = ir::I1;
        break;
      case ast::Binary::NEQ:
        op = ir::Binary::ICMP_NE;
        operand_type = ir::I32;
        result_type = ir::I1;
        break;
      case ast::Binary::AND:
        op = ir::Binary::AND;
        operand_type = ir::I1;
        result_type = ir::I1;
        break;
      case ast::Binary::OR:
        op = ir::Binary::OR;
        operand_type = ir::I1;
        result_type = ir::I1;
        break;
      }
      auto lhs = cast(add_expr(*expr.lhs), operand_type);
      auto rhs = cast(add_expr(*expr.rhs), operand_type);
      auto vreg = get_block()->push_back(ir::Binary{op, operand_type, lhs, rhs});
      return TypedOperand{result_type, vreg};
    },
    [this](const ast::Unary & expr) {
      switch (expr.op) {
      case ast::Unary::POS:
        return TypedOperand{ir::I32, cast(add_expr(*expr.operand), ir::I32)};
      case ast::Unary::NEG:
      {
        auto operand = cast(add_expr(*expr.operand), ir::I32);
        auto vreg = get_block()->push_back(ir::Binary{
          ir::Binary::SUB,
          ir::I32,
          ir::Const{0},
          operand
        });
        return TypedOperand{ir::I32, vreg};
      }
      case ast::Unary::NOT:
      {
        auto operand = add_expr(*expr.operand);
        auto vreg = get_block()->push_back(ir::Binary{
          ir::Binary::ICMP_EQ,
          operand.type,
          operand.inner,
          ir::Const{0}
        });
        return TypedOperand{ir::I1, vreg};
      }
      }
    },
    [this](const ast::FuncCall & expr) {
      auto & symbol = get_symbol(expr.func);
      if (symbol.kind != Symbol::FUNC) throw "variable used as a function";
      if (expr.args.size() != symbol.argc) throw "mismatched number of arguments";
      std::vector<std::pair<ir::Type, ir::Operand>> args;
      args.reserve(expr.args.size());
      for (auto & arg : expr.args) {
        args.emplace_back(ir::I32, cast(add_expr(arg), ir::I32));
      }
      auto vreg = get_block()->push_back(ir::Call{
        symbol.type,
        symbol.ir,
        std::move(args)
      });
      return TypedOperand{symbol.type, vreg};
    },
    [this](const ast::Ident & ident) {
      auto & symbol = get_symbol(ident);
      switch (symbol.kind) {
      case Symbol::CONST: return TypedOperand{symbol.type, symbol.ir};
      case Symbol::VAR:
      {
        auto vreg = get_block()->push_back(ir::Load{
          symbol.type,
          symbol.ir
        });
        return TypedOperand{symbol.type, vreg};
      }
      case Symbol::FUNC: throw "function used as a variable";
      }
    },
    [](int number) {
      return TypedOperand{ir::I32, ir::Const{number}};
    },
  }, expr);
}

ir::Operand Codegen::cast(const TypedOperand && operand, ir::Type type) {
  if (operand.type != type) {
    ir::InstrRef vreg;
    if (operand.type == ir::I1 && type == ir::I32) {
      vreg = get_block()->push_back(ir::Zext{ir::I1, operand.inner, ir::I32});
    } else if (operand.type == ir::I32 && type == ir::I1) {
      vreg = get_block()->push_back(ir::Binary{
        ir::Binary::ICMP_NE,
        ir::I32,
        operand.inner,
        ir::Const{0}
      });
    } else {
      throw "unsupported cast";
    }
    return vreg;
  } else {
    return operand.inner;
  }
}

const Symbol & Codegen::get_symbol(const ast::Ident & ident) {
  for (auto & scope : this->scopes | std::views::reverse) {
    auto symbol = scope.find(ident);
    if (symbol != scope.end()) return symbol->second;
  }
  if (ident == "getch") {
    return this->scopes.front().insert({
      "getch",
      Symbol{Symbol::FUNC, ir::I32, 0, ir::Global{"getch"}}
    }).first->second;
  } else if (ident == "putch") {
    return this->scopes.front().insert({
      "putch",
      Symbol{Symbol::FUNC, ir::VOID, 1, ir::Global{"putch"}}
    }).first->second;
  } else if (ident == "getint") {
    return this->scopes.front().insert({
      "getint",
      Symbol{Symbol::FUNC, ir::I32, 0, ir::Global{"getint"}}
    }).first->second;
  } else if (ident == "putint") {
    return this->scopes.front().insert({
      "putint",
      Symbol{Symbol::FUNC, ir::VOID, 1, ir::Global{"putint"}}
    }).first->second;
  }
  throw "can't find symbol";
}

int Codegen::eval_constexpr(const ast::Expr & expr) {
  return std::visit(overloaded {
    [this](const ast::Binary & expr) {
      switch (expr.op) {
      case ast::Binary::PLUS: return eval_constexpr(*expr.lhs) + eval_constexpr(*expr.rhs);
      case ast::Binary::MINUS: return eval_constexpr(*expr.lhs) - eval_constexpr(*expr.rhs);
      case ast::Binary::MULT: return eval_constexpr(*expr.lhs) * eval_constexpr(*expr.rhs);
      case ast::Binary::DIV: return eval_constexpr(*expr.lhs) / eval_constexpr(*expr.rhs);
      case ast::Binary::MOD: return eval_constexpr(*expr.lhs) % eval_constexpr(*expr.rhs);
      case ast::Binary::LT: return int(eval_constexpr(*expr.lhs) < eval_constexpr(*expr.rhs));
      case ast::Binary::LTEQ: return int(eval_constexpr(*expr.lhs) <= eval_constexpr(*expr.rhs));
      case ast::Binary::GT: return int(eval_constexpr(*expr.lhs) > eval_constexpr(*expr.rhs));
      case ast::Binary::GTEQ: return int(eval_constexpr(*expr.lhs) >= eval_constexpr(*expr.rhs));
      case ast::Binary::EQ: return int(eval_constexpr(*expr.lhs) == eval_constexpr(*expr.rhs));
      case ast::Binary::NEQ: return int(eval_constexpr(*expr.lhs) != eval_constexpr(*expr.rhs));
      case ast::Binary::AND: return int(eval_constexpr(*expr.lhs) && eval_constexpr(*expr.rhs));
      case ast::Binary::OR: return int(eval_constexpr(*expr.lhs) || eval_constexpr(*expr.rhs));
      }
    },
    [this](const ast::Unary & expr) {
      switch (expr.op) {
      case ast::Unary::POS: return eval_constexpr(*expr.operand);
      case ast::Unary::NEG: return -eval_constexpr(*expr.operand);
      case ast::Unary::NOT: return int(!eval_constexpr(*expr.operand));
      }
    },
    [](const ast::FuncCall & expr) -> int {
      throw "constant must be initialized with a constant expression";
    },
    [this](const ast::Ident & ident) -> int {
      auto & symbol = get_symbol(ident);
      switch (symbol.kind) {
      case Symbol::CONST:
        return std::get<ir::Const>(symbol.ir).value;
      case Symbol::VAR:
        throw "constant must be initialized with a constant expression";
      case Symbol::FUNC:
        throw "function used as a variable";
      }
    },
    [](int number) {
      return number;
    },
  }, expr);
}
