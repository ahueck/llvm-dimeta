// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

#include <stdlib.h>

struct A {
  float b;
  float c;
  int* a[2];
};

void foo(struct A* ar) {
  ar->a[1] = (int*)malloc(sizeof(int));
}
