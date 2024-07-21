#include <ostream>

#include "ir.hpp"
#include "overloaded.hpp"

std::ostream & operator<<(std::ostream & out, const ir::Program & program) {
  for (auto & def : program) {
    out << def;
  }
  return out;
}

std::ostream & operator<<(std::ostream & out, const ir::GlobalDef & def) {
  std::visit(overloaded {
    [&out](const ir::Func & func) {
      // first line
      out << "define dso_local " << func.rettype
          << " @" << func.name << '(';
      if (!func.args.empty()) {
        auto arg = func.args.begin();
        out << arg->type << ' ' << '%' << arg->vreg;
        for (arg++; arg != func.args.end(); arg++) {
          out << ", " << arg->type << ' ' << '%' << arg->vreg;
        }
      }
      out << ") {" << std::endl;
      // body
      auto block = func.blocks.begin();
      for (auto instr : *block) {
        out << instr;
      }
      for (block++; block != func.blocks.end(); block++) {
        out << std::endl;
        out << block->label << ":" << std::endl;
        for (auto instr : *block) {
          out << instr;
        }
      }
      // end
      out << "}";
    },
    [&out](const ir::GlobalVar & var) {
      out << '@' << var.name << " = dso_local global " << var.type << ' '
          << var.value;
    },
    [&out](const ir::FuncDecl & func) {
      // first line
      out << "declare " << func.rettype
          << " @" << func.name << '(';
      if (!func.args.empty()) {
        auto arg = func.args.begin();
        out << *arg;
        for (arg++; arg != func.args.end(); arg++) {
          out << ", " << *arg;
        }
      }
      out << ")";
    },
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
  case ir::Binary::ICMP_SLT: out << "icmp slt"; break;
  case ir::Binary::ICMP_SLE: out << "icmp sle"; break;
  case ir::Binary::ICMP_SGT: out << "icmp sgt"; break;
  case ir::Binary::ICMP_SGE: out << "icmp sge"; break;
  case ir::Binary::ICMP_EQ: out << "icmp eq"; break;
  case ir::Binary::ICMP_NE: out << "icmp ne"; break;
  case ir::Binary::AND: out << "and"; break;
  case ir::Binary::OR: out << "or"; break;
  }
  return out;
}

std::ostream & operator<<(std::ostream & out, const ir::Instr & instr) {
  out << "    ";
  std::visit(overloaded {
    [&out](const ir::Binary & instr) {
      out << '%' << instr.result << " = " << instr.op << ' '
          << instr.type << ' ' << instr.lhs << ", " << instr.rhs;
    },
    [&out](const ir::Ret & instr) {
      if (instr.type == ir::VOID) {
        out << "ret void";
      } else {
        out << "ret " << instr.type << ' ' << instr.retval;
      }
    },
    [&out](const ir::Alloca & instr) {
      out << '%' << instr.result << " = alloca " << instr.type;
    },
    [&out](const ir::Store & instr) {
      out << "store " << instr.type << ' ' << instr.from
          << ", ptr " << instr.ptr;
    },
    [&out](const ir::Load & instr) {
      out << '%' << instr.result << " = load " << instr.type
          << ", ptr " << instr.ptr;
    },
    [&out](const ir::Call & instr) {
      if (instr.type != ir::VOID) {
        out << '%' << instr.result << " = ";
      }
      out << "call " << instr.type << ' '
          << instr.func << '(';
      if (!instr.args.empty()) {
        auto arg = instr.args.begin();
        out << arg->type << ' ' << arg->value;
        for (arg++; arg != instr.args.end(); arg++) {
          out << ", " << arg->type << ' ' << arg->value;
        }
      }
      out << ')';
    },
    [&out](const ir::Zext & instr) {
      out << '%' << instr.result << " = zext "
          << instr.from_type << ' ' << instr.value
          << " to " << instr.to_type;
    },
    [&out](const ir::Br & instr) {
      out << "br label " << instr.dest;
    },
    [&out](const ir::BrCond & instr) {
      out << "br i1 " << instr.cond << ", label " << instr.iftrue
          << ", label " << instr.iffalse;
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
  case ir::VOID: out << "void"; break;
  case ir::I1: out << "i1"; break;
  case ir::I32: out << "i32"; break;
  case ir::PTR: out << "ptr"; break;
  case ir::LABEL: out << "label"; break;
  }
  return out;
}
