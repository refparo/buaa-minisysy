#include "ir_builder.hpp"
#include "overloaded.hpp"

ir::Program build(ast::Program ast) {
  return ir::Program{build(std::move(ast.func))};
}

ir::Func build(ast::Func ast) {
  ir::Func func{
    {},
    build(ast.rettype),
    std::move(ast.name),
    {ir::Block{}}
  };
  build_into(ast.body.stmt, func, func.blocks.front());
  return func;
}

ir::Type build(ast::Ident ast) {
  if (ast == "int") {
    return ir::SimpleType::I32;
  }
  throw "unsupported type";
}

void build_into(ast::Stmt & ast, ir::Func & func, ir::Block & block) {
  std::visit(overloaded{
    [&func, &block](ast::Return & ast) {
      ir::Operand retval = build_into(ast.retval, func, block);
      block.push_back(func.size());
      func.push_back(ir::Instr{ir::Ret{
        ir::SimpleType::I32,
        retval
      }});
    }
  }, ast);
}

ir::Binary::Op build(ast::Binary::Op op) {
  switch (op) {
  case ast::Binary::PLUS: return ir::Binary::ADD;
  case ast::Binary::MINUS: return ir::Binary::SUB;
  case ast::Binary::MULT: return ir::Binary::MUL;
  case ast::Binary::DIV: return ir::Binary::SDIV;
  case ast::Binary::MOD: return ir::Binary::SREM;
  }
}

// returns instr key of expr value
ir::Operand build_into(ast::Expr & ast, ir::Func & func, ir::Block & block) {
  return std::visit(overloaded{
    [&func, &block](ast::Binary & ast) {
      ir::Operand lhs = build_into(*ast.lhs, func, block);
      ir::Operand rhs = build_into(*ast.rhs, func, block);
      block.push_back(func.size());
      func.push_back(ir::Instr{ir::Binary{
        build(ast.op),
        ir::SimpleType::I32,
        lhs,
        rhs
      }});
      return ir::Operand{ir::Operand::INSTR, block.back()};
    },
    [&func, &block](ast::Unary & ast) {
      switch (ast.op) {
      case ast::Unary::POS:
        return build_into(*ast.operand, func, block);
      case ast::Unary::NEG:
        ir::Operand operand = build_into(*ast.operand, func, block);
        block.push_back(func.size());
        func.push_back(ir::Instr{ir::Binary{
          ir::Binary::SUB,
          ir::SimpleType::I32,
          ir::Operand{ir::Operand::CONST, 0},
          operand
        }});
        return ir::Operand{ir::Operand::INSTR, block.back()};
      }
    },
    [](int ast) {
      return ir::Operand{ir::Operand::CONST, ast};
    }
  }, ast);
}
