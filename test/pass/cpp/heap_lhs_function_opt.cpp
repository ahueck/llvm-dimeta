// RUN: %cpp-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

// REQUIRES: !18 && !14
// For the above compilers, the vector is only forward declared.

#include <cstdlib>
#include <vector>

std::vector<int*> ar;

int*& access(std::vector<int*>& ar) {
  return ar[1];
}

void foo(std::vector<int*>& ar) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK-NEXT: Pointer level: 1
  access(ar) = (int*)malloc(sizeof(int));
}
