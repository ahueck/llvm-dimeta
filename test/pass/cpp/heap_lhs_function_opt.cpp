// RUN: %cpp-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include <cstdlib>
#include <vector>

std::vector<int*> ar; // without, clang 14 & 18 do a fwd declare of debug data for vectors

int*& access(std::vector<int*>& ar) {
  return ar[77];
}

void foo(std::vector<int*>& ar) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  access(ar) = (int*)malloc(sizeof(int));
}
