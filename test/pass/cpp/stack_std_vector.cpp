// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

#include <vector>

double foo() {
  std::vector<double> vec_double = {1., 2.};
  return vec_double[0];
}
