// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)

#include <stdlib.h>

struct B {
  double* x;
};

struct A {
  float b;
  float c;
  struct B* a[2];
};

void foo(struct A* ar) {
  ar->a[1]->x = (double*)malloc(sizeof(double));

  //  ar->a[0] = (struct B*)malloc(sizeof(struct B));
}
