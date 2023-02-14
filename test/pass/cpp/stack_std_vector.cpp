// RUN: %cpp-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

#include <vector>

double foo() {
  std::vector<double> vec_double = {1., 2.};
  return vec_double[0];
}
