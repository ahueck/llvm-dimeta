// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <cstdlib>
#include <vector>

int*& access(std::vector<int*>& ar) {
  return ar[1];
}

void foo(std::vector<int*>& ar) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK-NEXT: Pointer level: 1
  access(ar) = (int*)malloc(sizeof(int));
}
