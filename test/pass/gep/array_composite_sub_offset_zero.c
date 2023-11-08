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
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF1:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF1]] = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
  // CHECK-NEXT: Pointer level: 1
  ar->a[0]->x = (double*)malloc(sizeof(double));
}

void bar(struct A* as) {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "B"
  // CHECK-NEXT: Pointer level: 1
  as->a[0] = (struct B*)malloc(sizeof(struct B));
}
