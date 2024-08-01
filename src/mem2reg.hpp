#include <algorithm>
#include <functional>
#include <map>

#include "ir.hpp"

template<typename Node>
using AdjList = std::map<Node, std::vector<Node>>;

// Calculates the inverse of CFG with nullptr added as an initial node.
AdjList<ir::Label> inverse_cfg(ir::Func & func);

template<typename Block>
std::vector<Block> postorder(const AdjList<Block> & cfg, Block initial) {
  std::vector<Block> result;
  std::vector<Block> stack;
  std::function<void()> go = [&go, &result, &stack, &cfg]() {
    for (auto pred : cfg.at(stack.back())) {
      if (
        std::find(result.begin(), result.end(), pred) == result.end() &&
        std::find(stack.begin(), stack.end(), pred) == stack.end()
      ) {
        stack.push_back(pred);
        go();
        stack.pop_back();
      }
    }
    result.push_back(stack.back());
  };
  stack.push_back(initial);
  go();
  return result;
}

// Calculate the Dom sets.
// For each `block` in the CFG, `dom_sets[block]` is the path on the dom tree
// from the initial block to `block`.
// Thus, `*++dom_sets[block].rbegin() == IDom(block)`
// except for the initial block.
template<typename Block>
AdjList<Block> dominator_sets(
  Block initial,
  const AdjList<Block> & inv_cfg,
  const std::vector<Block> & order
) {
  AdjList<Block> result;
  result.emplace(initial, std::vector{initial});
  while (true) {
    bool changed = false;
    for (auto block : order) {
      auto & dom = result[block];
      if (!dom.empty()) {
        dom.pop_back();
      }
      // dom <- ∩_{pred in pred(block)} Dom(pred)
      for (auto pred : inv_cfg.at(block)) {
        // if pred != {all blocks}
        if (auto it = result.find(pred); it != result.end()) {
          auto & dom_pred = it->second;
          if (dom.empty()) {
            // if dom == {all blocks}, dom <- dom_pred
            dom = dom_pred;
            changed = true;
          } else {
            // otherwise, dom <- dom ∩ dom_pred
            // find common prefix
            auto lhs = dom.begin();
            auto rhs = dom_pred.begin();
            while (
              lhs != dom.end() &&
              rhs != dom_pred.end() &&
              *lhs == *rhs
            ) {
              lhs++; rhs++;
            }
            // cut the different part
            if (lhs != dom.end()) {
              dom.resize(lhs - dom.begin());
              changed = true;
            }
          }
        }
      }
      dom.push_back(block); // now dom == Dom(block)
    }
    if (!changed) break;
  }
  return result;
}

void mem2reg(ir::Func & func);
