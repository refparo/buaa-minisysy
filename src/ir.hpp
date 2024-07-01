#pragma once

#include <list>
#include <string>
#include <variant>
#include <vector>

namespace ir {

struct Operand {
  enum Kind {
    CONST, INSTR
  } kind;
  int value;
};

enum SimpleType {
  VOID, I1, I32, LABEL
};

using Type = std::variant<SimpleType>;

struct Binary {
  enum Op {
    ADD, SUB, MUL, SDIV, SREM
  } op;
  Type type;
  Operand lhs;
  Operand rhs;
};

struct Ret {
  Type type;
  Operand retval;
};

struct Instr : std::variant<Binary, Ret> {
  int vreg = 0;
};

struct Block : std::list<int> {
  int vreg = 0;
};

struct Func : std::vector<Instr> {
  Type rettype;
  std::string name;
  std::vector<Block> blocks;
};

using Program = std::vector<Func>;

}

void print(std::ostream & out, const ir::Operand & operand, const ir::Func & func);
void print(std::ostream & out, const ir::Instr & instr, const ir::Func & func);
std::ostream & operator<<(std::ostream & out, const ir::Type & type);
std::ostream & operator<<(std::ostream & out, const ir::Func & func);
std::ostream & operator<<(std::ostream & out, const ir::Program & program);
