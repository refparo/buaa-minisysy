#include <ostream>

#include "ir.hpp"
#include "overloaded.hpp"

std::ostream & operator<<(std::ostream & out, const ir::Program & program) {
  auto func = program.begin();
  out << *func;
  for (func++; func != program.end(); func++) {
    out << std::endl;
    out << *func;
  }
  return out;
}

std::ostream & operator<<(std::ostream & out, const ir::Func & func) {
  int vreg_counter = 0;
  out << "define dso_local " << func.rettype
      << " @" << func.name << "() {" << std::endl;
  for (auto block = func.blocks.begin(); block != func.blocks.end(); block++) {
    if (block->vreg > 0) {
      out << std::endl;
      out << block->vreg << ":" << std::endl;
    }
    for (auto key : *block) {
      auto & instr = func[key];
      print(out, instr, func);
    }
  }
  out << "}" << std::endl;
  return out;
}

std::ostream & operator<<(std::ostream & out, const ir::Type & type) {
  out << std::visit(overloaded{
    [](const ir::SimpleType & type) {
      switch (type) {
      case ir::VOID: return "void";
      case ir::I1: return "i1";
      case ir::I32: return "i32";
      case ir::LABEL: return "label";
      }
    }
  }, type);
  return out;
}

std::ostream & operator<<(std::ostream & out, ir::Binary::Op op) {
  switch (op) {
  case ir::Binary::ADD: out << "add"; break;
  case ir::Binary::SUB: out << "sub"; break;
  case ir::Binary::MUL: out << "mul"; break;
  case ir::Binary::SDIV: out << "sdiv"; break;
  case ir::Binary::SREM: out << "srem"; break;
  }
  return out;
}

void print(std::ostream & out, const ir::Instr & instr, const ir::Func & func) {
  std::visit(overloaded{
    [&out, &instr, &func](const ir::Binary & binary) {
      out << "    %" << instr.vreg << " = " << binary.op << " " << binary.type << " ";
      print(out, binary.lhs, func);
      out << ", ";
      print(out, binary.rhs, func);
      out << std::endl;
    },
    [&out, &func](const ir::Ret & ret) {
      out << "    ret " << ret.type << " ";
      print(out, ret.retval, func);
      out << std::endl;
    }
  }, instr);
}

void print(std::ostream & out, const ir::Operand & operand, const ir::Func & func) {
  switch (operand.kind) {
  case ir::Operand::CONST: out << operand.value; break;
  case ir::Operand::INSTR: out << "%" << func[operand.value].vreg; break;
  }
}
