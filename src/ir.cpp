#include <ostream>

#include "ir.hpp"
#include "overloaded.hpp"

constexpr bool has_result(const ir::Instr & instr) {
  return std::visit(overloaded {
    [](const ir::Binary & instr) { return true; },
    [](const ir::Alloca & instr) { return true; },
    [](const ir::Store & instr) { return false; },
    [](const ir::Load & instr) { return true; },
    [](const ir::Call & instr) { return instr.type == ir::I32; },
    [](const ir::Zext & instr) { return true; },
    [](const ir::Phi & instr) { return true; },
  }, instr);
}

void assign_vregs(ir::Func & func) {
  int vreg = int(func.args.size());
  for (auto & block : func.blocks) {
    block.label = vreg++;
    for (auto & instr : block.body) {
      if (has_result(instr)) {
        instr.vreg = vreg++;
      }
    }
  }
}

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
        out << func.args[0] << ' ' << "%0";
        for (int i = 1; i < func.args.size(); i++) {
          out << ", " << func.args[i] << ' ' << '%' << i;
        }
      }
      out << ") {" << std::endl;
      // body
      auto block = func.blocks.begin();
      out << *block;
      for (block++; block != func.blocks.end(); block++) {
        out << std::endl;
        out << block->label << ':' << std::endl;
        out << *block;
      }
      // end
      out << "}";
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
    [&out](const ir::GlobalVar & var) {
      out << '@' << var.name << " = dso_local global " << var.type << ' '
          << var.value;
    },
  }, def);
  return out << std::endl;
}

std::ostream & operator<<(std::ostream & out, const ir::Block & block) {
  for (auto & instr : block.body) {
    out << "    ";
    if (has_result(instr)) {
      out << '%' << instr.vreg << " = ";
    }
    out << instr;
  }
  out << "    " << block.terminator;
  return out;
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
  std::visit(overloaded {
    [&out](const ir::Binary & instr) {
      out << instr.op << ' ' << instr.type << ' '
          << instr.lhs << ", " << instr.rhs;
    },
    [&out](const ir::Ret & instr) {
      if (instr.type == ir::VOID) {
        out << "ret void";
      } else {
        out << "ret " << instr.type << ' ' << instr.retval;
      }
    },
    [&out](const ir::Alloca & instr) {
      out << "alloca " << instr.type;
    },
    [&out](const ir::Store & instr) {
      out << "store " << instr.type << ' ' << instr.from
          << ", ptr " << instr.ptr;
    },
    [&out](const ir::Load & instr) {
      out << "load " << instr.type << ", ptr " << instr.ptr;
    },
    [&out](const ir::Call & instr) {
      out << "call " << instr.type << ' '
          << instr.func << '(';
      if (!instr.args.empty()) {
        auto arg = instr.args.begin();
        out << arg->first << ' ' << arg->second;
        for (arg++; arg != instr.args.end(); arg++) {
          out << ", " << arg->first << ' ' << arg->second;
        }
      }
      out << ')';
    },
    [&out](const ir::Zext & instr) {
      out << "zext " << instr.from_type << ' ' << instr.value
          << " to " << instr.to_type;
    },
    [&out](const ir::Phi & instr) {
      out << "phi " << instr.type << ' ';
      auto source = instr.sources.begin();
      out << '[' << source->first << ", " << source->second << ']';
      for (source++; source != instr.sources.end(); source++) {
        out << ", [" << source->first << ", " << source->second << ']';
      }
    },
  }, instr);
  return out << std::endl;
}

std::ostream & operator<<(std::ostream & out, const ir::Terminator & instr) {
  std::visit(overloaded {
    [&out](std::monostate _) {
      throw "block not terminated!";
    },
    [&out](const ir::Ret & instr) {
      if (instr.type == ir::VOID) {
        out << "ret void";
      } else {
        out << "ret " << instr.type << ' ' << instr.retval;
      }
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
    [&out](const ir::Result operand) {
      out << '%' << operand->vreg;
    },
    [&out](const ir::Arg operand) {
      out << '%' << operand.idx;
    },
    [&out](const ir::Global & operand) {
      out << '@' << operand;
    },
  }, operand);
  return out;
}

std::ostream & operator<<(std::ostream & out, const ir::Label & label) {
  return out << '%' <<  label->label;
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
