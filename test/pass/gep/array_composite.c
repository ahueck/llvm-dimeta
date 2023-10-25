// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

struct B {};

struct A {
  float b;
  float c;
  struct B* a[2];
};

void foo(struct A* ar) {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "B"
  ar->a[1] = (struct B*)malloc(sizeof(struct B));
}
