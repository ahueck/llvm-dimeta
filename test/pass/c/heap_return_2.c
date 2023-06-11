// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

#include <stdlib.h>

int* foo(int n) {
  int* p = malloc(sizeof(int) * n);
  return p;
}
