#include "codegen.hpp"
#include "overloaded.hpp"

Codegen::Codegen() {}

ir::Program Codegen::get() const && {
  return this->ir;
}

// TODO
