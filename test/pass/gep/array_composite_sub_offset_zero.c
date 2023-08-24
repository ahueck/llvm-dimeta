// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

struct B {
  double* x;
};

struct A {
  float b;
  float c;
  struct B* a[2];
};

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
void foo(struct A* ar) {
  ar->a[0]->x = (double*)malloc(sizeof(double));
}

// CHECK: Final Type: {{.*}} = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "B"
void bar(struct A* as) {
  as->a[0] = (struct B*)malloc(sizeof(struct B));
}
