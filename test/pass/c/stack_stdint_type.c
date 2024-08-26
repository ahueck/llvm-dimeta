// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

#include "stdint.h"

int foo() {
  const int_least64_t a = 0;
  return a;
}

// CHECK: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: long, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:   Qualifiers:      [ const ]
