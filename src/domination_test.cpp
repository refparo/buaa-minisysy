#include <iostream>

#include "mem2reg.hpp"

using namespace std;

int main() {
  AdjList<int> inv_cfg{
    {1, {0, 3}},
    {2, {1}},
    {3, {2, 7}},
    {4, {3}},
    {5, {1}},
    {6, {5}},
    {7, {6, 8}},
    {8, {5}},
    {0, {4}},
  };
  auto order = postorder(inv_cfg, 0);
  inv_cfg.erase(0);
  order.pop_back();
  cout << "Dom:" << endl;
  auto dom = dominator_sets(0, inv_cfg, order);
  for (auto & [i, js] : dom) {
    cout << i << ": ";
    for (auto j : js) {
      cout << j << ", ";
    }
    cout << endl;
  }
  cout << "DF:" << endl;
  auto df = domination_frontiers(inv_cfg, dom);
  for (auto & [i, js] : df) {
    cout << i << ": ";
    for (auto j : js) {
      cout << j << ", ";
    }
    cout << endl;
  }
  return 0;
}
