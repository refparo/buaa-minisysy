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
    ADD, SUB, MUL, SDIV, SREM
  } op;
  Type type;
  int result = 0;
  Operand lhs;
  Operand rhs;
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
  int result = 0;
  Operand ptr;
};

struct Arg {
  Type type;
  Operand value;
};

struct Call {
  Type type;
  int result = 0;
  Operand func;
  std::vector<Arg> args;
};

using Instr = std::variant<Binary, Ret, Alloca, Store, Load, Call>;

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

using GlobalDef = std::variant<Func, GlobalVar>;

using Program = std::vector<GlobalDef>;

}

std::ostream & operator<<(std::ostream & out, const ir::Type & type);
std::ostream & operator<<(std::ostream & out, const ir::Operand & operand);
std::ostream & operator<<(std::ostream & out, const ir::Instr & instr);
std::ostream & operator<<(std::ostream & out, const ir::GlobalDef & def);
std::ostream & operator<<(std::ostream & out, const ir::Program & program);
