// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s
// CHECK: Final Type: {{.*}} = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "A"

#include <stdlib.h>

struct A {};

void foo(struct A** ar) {
  ar[0] = (struct A*)malloc(sizeof(struct A));
}
