// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %cpp-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include <cstdlib>
#include <vector>

int*& access(std::vector<int*>& ar) {
  return ar[1];
}

void foo(std::vector<int*>& ar) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK-NEXT: Pointer level: 1 (T*)
  access(ar) = (int*)malloc(sizeof(int));
}
