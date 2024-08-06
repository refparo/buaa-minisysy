#include <set>

#include "mem2reg.hpp"
#include "overloaded.hpp"

AdjList<ir::Label> inverse_cfg(ir::Func & func) {
  AdjList<ir::Label> result;
  for (auto & block : func.blocks) {
    result[&block];
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

void mem2reg(ir::Func & func, const AdjList<ir::Label> & df) {
  // ir::Instr* references the variables (merely used as keys; should not be dereferenced)
  // first = is loaded across block; second = is stored in blocks
  std::map<ir::Instr*, std::pair<bool, std::vector<ir::Label>>> var_blocks;
  for (auto & block : func.blocks) {
    std::vector<ir::Instr*> dead;
    for (auto & instr : block.body) {
      std::visit(overloaded{
        [&var_blocks, &block, &dead](ir::Store & instr) {
          if (std::holds_alternative<ir::Result>(instr.ptr)) {
            auto var = &*std::get<ir::Result>(instr.ptr);
            dead.push_back(var);
            var_blocks[var].second.push_back(&block);
          }
        },
        [&var_blocks, &dead](ir::Load & instr) {
          if (std::holds_alternative<ir::Result>(instr.ptr)) {
            auto var = &*std::get<ir::Result>(instr.ptr);
            if (std::find(dead.begin(), dead.end(), var) == dead.end()) {
              var_blocks[var].first = true;
            }
          }
        },
        [](auto & _) {},
      }, instr);
    }
  }
  std::map<ir::Label, std::map<ir::Instr*, ir::InstrRef>> phi_map;
  for (auto & [var, pair] : var_blocks) {
    auto & [global, blocks] = pair;
    if (global) {
      auto todo = blocks;
      while (!todo.empty()) {
        auto next = todo.back();
        todo.pop_back();
        if (auto it = df.find(next); it != df.end()) {
          for (auto block : it->second) {
            auto & phi = phi_map[block][var];
            if (phi == ir::InstrRef{}) {
              block->body.emplace_front(ir::Phi{ir::I32});
              phi = block->body.begin();
              todo.push_back(block);
            }
          }
        }
      }
    }
  }
  std::set<ir::Label> done;
  std::function<void(ir::Label, std::map<ir::Instr*, ir::Operand>)> go =
  [&phi_map, &done, &go](ir::Label block, std::map<ir::Instr*, ir::Operand> var_values) {
    done.insert(block);
    auto & phis = phi_map[block];
    for (auto [var, phi] : phis) {
      var_values.at(var) = phi;
    }
    auto it = block->body.begin();
    for (int i = 0; i < phis.size(); i++) {
      it++;
    }
    for (; it != block->body.end(); it++) {
      std::visit(overloaded{
        [&block, &it](ir::Alloca & instr) {
          block->body.erase(it--);
        },
        [&block, &var_values, &it](ir::Store & instr) {
          if (std::holds_alternative<ir::Result>(instr.ptr)) {
            var_values.at(&*std::get<ir::Result>(instr.ptr)) = instr.from;
            block->body.erase(it--);
          }
        },
        [&var_values, &it](ir::Load & instr) {
          if (std::holds_alternative<ir::Result>(instr.ptr)) {
            *it = ir::Binary{
              ir::Binary::ADD, ir::I32,
              var_values.at(&*std::get<ir::Result>(instr.ptr)),
              ir::Const{0}
            };
          }
        },
        [](auto & _) {},
      }, *it);
    }
    auto flow_out = [&phi_map, &done, &go, &block, &var_values](ir::Label succ) {
      for (auto [var, phi] : phi_map[succ]) {
        auto value = var_values.at(var);
        std::get<ir::Phi>(*phi).sources.emplace_back(value, block);
      }
      if (!done.contains(succ)) {
        go(succ, var_values);
      }
    };
    std::visit(overloaded{
      [&flow_out](ir::Br & instr) {
        flow_out(instr.dest);
      },
      [&flow_out](ir::BrCond & instr) {
        flow_out(instr.iftrue);
        flow_out(instr.iffalse);
      },
      [](auto & _) {},
    }, block->terminator);
  };
  std::map<ir::Instr*, ir::Operand> var_values;
  for (auto & [var, _] : var_blocks) {
    var_values[var] = ir::Const{0};
  }
  go(&func.blocks.front(), var_values);
}
