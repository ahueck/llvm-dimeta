// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 -yaml | %filecheck %s

// CHECK-NOT: Vtable:
// CHECK: Yaml Verifier: 1

#include <vector>

void foo() {
  std::vector<double> vec_double;  // = {1., 2.};
  //  return vec_double[0];
}
