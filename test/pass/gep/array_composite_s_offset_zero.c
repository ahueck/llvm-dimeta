// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

struct A {};

void foo(struct A** ar) {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "A"
  // CHECK-NEXT: Pointer level: 1
  ar[0] = (struct A*)malloc(sizeof(struct A));
}
