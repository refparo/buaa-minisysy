#pragma once

#include <list>
#include <string>
#include <variant>
#include <vector>

namespace ir {

enum Type {
  VOID, I1, I32, PTR, LABEL
};

// Operand
using BlockBody = std::list<struct Instr>;
using FuncBody = std::list<struct Block>;

using InstrRef = BlockBody::iterator;
using Label = FuncBody::iterator;

struct Const { int value; };
using Var = InstrRef;
struct Arg { int idx; };
using Global = std::string;
using Operand = std::variant<Const, Var, Arg, Global>;

// instr

struct Binary {
  enum Op {
    ADD, SUB, MUL, SDIV, SREM,
    ICMP_SLT, ICMP_SLE, ICMP_SGT, ICMP_SGE, ICMP_EQ, ICMP_NE,
    AND, OR,
  } op;
  Type type;
  Operand lhs;
  Operand rhs;
};

struct Alloca {
  Type type;
};

struct Store {
  Type type;
  Operand from;
  Operand ptr;
};

struct Load {
  Type type;
  Operand ptr;
};

struct Call {
  Type type;
  Operand func;
  std::vector<std::pair<Type, Operand>> args;
};

struct Zext {
  Type from_type;
  Operand value;
  Type to_type;
};

struct Phi {
  Type type;
  std::vector<std::pair<Operand, Label>> sources;
};

using Using_Instr = std::variant<
  Alloca,
  Store,
  Load,
  Binary,
  Call,
  Zext,
  Phi
>;
struct Instr : Using_Instr {
  int vreg;

  using Using_Instr::Using_Instr;
};

// terminator

struct Ret {
  Type type;
  Operand retval;
};

struct Br {
  Label dest;
};

struct BrCond {
  Operand cond;
  Label iftrue;
  Label iffalse;
};

// std::monostate means no terminator
using Terminator = std::variant<std::monostate, Ret, Br, BrCond>;

// program

struct Block {
  BlockBody body;
  Terminator terminator;
  int label;

  [[nodiscard]] inline bool terminated() const {
    return terminator.index() != 0;
  }

  [[nodiscard]] inline bool empty() const {
    return body.empty() && !terminated();
  }

  inline InstrRef push_back(Instr && instr) {
    return body.insert(body.end(), instr);
  }
};

struct Func {
  Type rettype;
  std::string name;
  std::vector<Type> args;
  FuncBody blocks;

  inline Label new_block() {
    return blocks.insert(blocks.end(), Block{});
  }
};

struct FuncDecl {
  Type rettype;
  std::string name;
  std::vector<Type> args;
};

struct GlobalVar {
  std::string name;
  Type type;
  int value;
};

using GlobalDef = std::variant<Func, FuncDecl, GlobalVar>;

using Program = std::vector<GlobalDef>;

}

void assign_vregs(ir::Program & program);

std::ostream & operator<<(std::ostream & out, const ir::Type & type);
std::ostream & operator<<(std::ostream & out, const ir::Operand & operand);
std::ostream & operator<<(std::ostream & out, const ir::Label & label);
std::ostream & operator<<(std::ostream & out, const ir::Instr & instr);
std::ostream & operator<<(std::ostream & out, const ir::Terminator & instr);
std::ostream & operator<<(std::ostream & out, const ir::Block & block);
std::ostream & operator<<(std::ostream & out, const ir::GlobalDef & def);
std::ostream & operator<<(std::ostream & out, const ir::Program & program);
