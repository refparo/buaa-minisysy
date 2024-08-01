#include "mem2reg.hpp"
#include "overloaded.hpp"

AdjList<ir::Label> inverse_cfg(ir::Func & func) {
  AdjList<ir::Label> result;
  for (auto & block : func.blocks) {
    std::visit(overloaded{
      [](std::monostate _) {},
      [&result, &block](ir::Ret & _) {
        // end nodes
        result[nullptr].push_back(&block);
      },
      [&result, &block](ir::Br & instr) {
        result[instr.dest].push_back(&block);
      },
      [&result, &block](ir::BrCond & instr) {
        result[instr.iftrue].push_back(&block);
        result[instr.iffalse].push_back(&block);
      },
    }, block.terminator);
  }
  return result;
}

static void mem2reg(ir::Func & func, ir::InstrRef var) {

}

void mem2reg(ir::Func & func) {
  for (auto & block : func.blocks) {
    for (auto instr = block.body.begin(); instr != block.body.end(); instr++) {
      if (std::holds_alternative<ir::Alloca>(*instr)) {
        mem2reg(func, instr);
      }
    }
  }
}
