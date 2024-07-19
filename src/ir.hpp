#pragma once

#include <string>
#include <variant>
#include <vector>

namespace ir {

enum Type {
  VOID, I1, I32, PTR, LABEL
};

struct Const { int value; };
struct VReg { int vreg; };
struct Global { std::string name; };
using Operand = std::variant<Const, VReg, Global>;

struct Binary {
  enum Op {
    ADD, SUB, MUL, SDIV, SREM,
    ICMP_SLT, ICMP_SLE, ICMP_SGT, ICMP_SGE, ICMP_EQ, ICMP_NE,
    AND, OR,
  } op;
  Type type;
  Operand lhs;
  Operand rhs;
  int result = 0;
};

struct Ret {
  Type type;
  Operand retval;
};

struct Alloca {
  Type type;
  int result = 0;
};

struct Store {
  Type type;
  Operand from;
  Operand ptr;
};

struct Load {
  Type type;
  Operand ptr;
  int result = 0;
};

struct Arg {
  Type type;
  Operand value;
};

struct Call {
  Type type;
  Operand func;
  std::vector<Arg> args;
  int result = 0;
};

struct Zext {
  Type from_type;
  Operand value;
  Type to_type;
  int result = 0;
};

struct Br {
  Operand dest;
};

struct BrCond {
  Operand cond;
  Operand iftrue;
  Operand iffalse;
};

using Instr = std::variant<
  Binary,
  Ret,
  Alloca,
  Store,
  Load,
  Call,
  Zext,
  Br,
  BrCond
>;

struct Block : std::vector<Instr> {
  int label = 0;
};

struct ArgDef {
  Type type;
  int vreg = 0;
};

struct Func {
  Type rettype;
  std::string name;
  std::vector<ArgDef> args;
  std::vector<Block> blocks;
};

struct GlobalVar {
  std::string name;
  Type type;
  int value;
};

struct FuncDecl {
  Type rettype;
  std::string name;
  std::vector<Type> args;
};

using GlobalDef = std::variant<Func, GlobalVar, FuncDecl>;

using Program = std::vector<GlobalDef>;

}

std::ostream & operator<<(std::ostream & out, const ir::Type & type);
std::ostream & operator<<(std::ostream & out, const ir::Operand & operand);
std::ostream & operator<<(std::ostream & out, const ir::Instr & instr);
std::ostream & operator<<(std::ostream & out, const ir::GlobalDef & def);
std::ostream & operator<<(std::ostream & out, const ir::Program & program);
