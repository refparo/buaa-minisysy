#include <iostream>

#include "parser.hpp"
#include "codegen.hpp"
#include "mem2reg.hpp"

int main() {
  try {
    Lexer lexer{std::cin};
    auto ast = parse(lexer);
    Codegen codegen;
    codegen.add_program(ast);
    auto program = std::move(codegen).get();
    foreach_func(program, [](ir::Func & func) {
      auto inv_cfg = inverse_cfg(func);
      auto order = postorder(inv_cfg, (ir::Block *)nullptr);
      inv_cfg.erase(nullptr);
      order.pop_back();
      auto dom = dominator_sets(&func.blocks.front(), inv_cfg, order);
      auto df = domination_frontiers(inv_cfg, dom);
      mem2reg(func, df);
      assign_vregs(func);
    });
    std::cout << program;
  } catch (const char * err) {
    std::cout << err << std::endl;
    return 1;
  }
  return 0;
}
