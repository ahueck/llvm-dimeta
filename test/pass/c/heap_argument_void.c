// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

void foo(void** p, int n) {
  // CHECK: Extracted Type: !14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
  // CHECK: Final Type: !14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
  *p = malloc(sizeof(int) * n);
}
