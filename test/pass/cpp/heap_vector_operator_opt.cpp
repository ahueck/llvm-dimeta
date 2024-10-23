// RUN: %cpp-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

// This compilers fwd declare the vector debug data
// REQUIRES: !18 && !14 && !15

#include <cstdlib>
#include <vector>

void foo(std::vector<int*>& ar) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK-NEXT: Pointer level: 1 (T*)
  ar[2] = (int*)malloc(sizeof(int));
}
