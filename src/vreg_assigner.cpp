#include "overloaded.hpp"
#include "vreg_assigner.hpp"

void assign(ir::Program & program) {
  for (auto & func : program) {
    assign(func);
  }
}

void assign(ir::Func & func) {
  int vreg = 0;
  for (auto & block : func.blocks) {
    if (vreg > 0) {
      vreg++;
      block.vreg = vreg;
    }
    for (auto idx : block) {
      auto & instr = func[idx];
      std::visit(overloaded{
        [&vreg, &instr](ir::Binary & binary) {
          vreg++;
          instr.vreg = vreg;
        },
        [](ir::Ret & ret) {}
      }, instr);
    }
  }
}
