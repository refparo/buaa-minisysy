#include <iostream>

#include "parser.hpp"
#include "ir_builder.hpp"
#include "vreg_assigner.hpp"

int main() {
  try {
    Parser parser{Lexer{std::cin}};
    ast::Program ast = parser.parse();
    ir::Program ir = build(std::move(ast));
    assign(ir);
    std::cout << ir;
  } catch (const char * err) {
    std::cout << err << std::endl;
    return 1;
  }
  return 0;
}
