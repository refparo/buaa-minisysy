#include <ostream>

#include "ir.hpp"
#include "overloaded.hpp"

std::ostream & operator<<(std::ostream & out, const ir::Program & program) {
  auto def = program.begin();
  out << *def;
  for (def++; def != program.end(); def++) {
    out << std::endl;
    out << *def;
  }
  return out;
}

std::ostream & operator<<(std::ostream & out, const ir::GlobalDef & def) {
  std::visit(overloaded {
    [&out](const ir::Func & func) {
      // first line
      out << "define dso_local " << func.rettype
          << " @" << func.name << '(';
      auto arg = func.args.begin();
      out << arg->type << ' ' << '%' << arg->vreg;
      for (; arg != func.args.end(); arg++) {
        out << ", " << arg->type << ' ' << '%' << arg->vreg;
      }
      out << ") {" << std::endl;
      // body
      for (auto block : func.blocks) {
        if (block.label > 0) {
          out << std::endl;
          out << block.label << ":" << std::endl;
        }
        for (auto instr : block) {
          out << instr;
        }
        out << "}";
      }
    },
    [&out](const ir::GlobalVar & var) {
      out << '@' << var.name << " = dso_local global " << var.type << ' '
          << var.type;
    }
  }, def);
  return out << std::endl;
}

static std::ostream & operator<<(std::ostream & out, ir::Binary::Op op) {
  switch (op) {
  case ir::Binary::ADD: out << "add"; break;
  case ir::Binary::SUB: out << "sub"; break;
  case ir::Binary::MUL: out << "mul"; break;
  case ir::Binary::SDIV: out << "sdiv"; break;
  case ir::Binary::SREM: out << "srem"; break;
  }
  return out;
}

std::ostream & operator<<(std::ostream & out, const ir::Instr & instr) {
  out << "    ";
  std::visit(overloaded {
    [&out](const ir::Binary & instr) {
      out << instr.result << " = " << instr.op << ' ' << instr.type << ' '
          << instr.lhs << ", " << instr.rhs;
    },
    [&out](const ir::Ret & instr) {
      out << "ret " << instr.type << ' ' << instr.retval;
    },
    [&out](const ir::Alloca & instr) {
      out << instr.result << " = alloca " << instr.type;
    },
    [&out](const ir::Store & instr) {
      out << "store " << instr.type << ' ' << instr.from << ", ptr " << instr.ptr;
    },
    [&out](const ir::Load & instr) {
      out << instr.result << " = load " << instr.type << ", ptr " << instr.ptr;
    },
    [&out](const ir::Call & instr) {
      out << instr.result << " = call " << instr.type << ' ' << instr.func << '(';
      auto arg = instr.args.begin();
      out << arg->type << ' ' << arg->value;
      for (; arg != instr.args.end(); arg++) {
        out << ", " << arg->type << ' ' << arg->value;
      }
      out << ')';
    },
  }, instr);
  return out << std::endl;
}

std::ostream & operator<<(std::ostream & out, const ir::Operand & operand) {
  std::visit(overloaded {
    [&out](const ir::Const operand) {
      out << operand.value;
    },
    [&out](const ir::VReg operand) {
      out << '%' << operand.vreg;
    },
    [&out](const ir::Global operand) {
      out << '@' << operand.name;
    },
  }, operand);
  return out;
}

std::ostream & operator<<(std::ostream & out, const ir::Type & type) {
  switch (type) {
  case ir::VOID: out << "void";
  case ir::I1: out << "i1";
  case ir::I32: out << "i32";
  case ir::PTR: out << "ptr";
  case ir::LABEL: out << "label";
  }
  return out;
}
