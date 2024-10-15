// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

// Needs TBAA analysis for clang 15

struct B {
  double* x;
};

struct A {
  float b;
  float c;
  struct B* a[2];
};

void foo(struct A* ar) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
  ar->a[0]->x = (double*)malloc(sizeof(double));
}

void bar(struct A* as) {
  // CHECK: Final Type: {{.*}} = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "B"
  as->a[0] = (struct B*)malloc(sizeof(struct B));
}
