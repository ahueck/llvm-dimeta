// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "B"

#include <stdlib.h>

struct B {};

struct A {
  float b;
  float c;
  struct B* a[2];
};

void foo(struct A* ar) {
  ar->a[1] = (struct B*)malloc(sizeof(struct B));
}
