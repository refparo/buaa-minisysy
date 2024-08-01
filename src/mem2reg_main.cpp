#include <iostream>

#include "mem2reg.hpp"

int main() {
  AdjList<int> inv_cfg{
    {1, {0}},
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
  order.pop_back();
  auto dom = dominator_sets(0, inv_cfg, order);
  for (int i = 0; i <= 8; i++) {
    using namespace std;
    cout << i << ":";
    for (auto j : dom[i]) {
      cout << j << ", ";
    }
    cout << endl;
  }
  return 0;
}
