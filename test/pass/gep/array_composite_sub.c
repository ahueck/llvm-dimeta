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

void foo(struct A* ar) {
  // clang-format off
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
  // CHECK-NEXT: Pointer level: 1
  // clang-format on
  ar->a[1]->x = (double*)malloc(sizeof(double));
}
