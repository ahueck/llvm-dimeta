// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

#include <stdlib.h>

struct A {
  int* a;
};

void foo(int** ar) {
  ar[2] = (int*)malloc(sizeof(int));
}
