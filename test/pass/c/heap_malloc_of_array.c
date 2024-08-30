// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

void foo(int n) {
  int(*array)[3] = malloc(2 * sizeof(int[3]));
}

// CHECK:   Line:            6
// CHECK: Fundamental:{{ *}}{ Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT: Array:{{ *}}3
// CHECK-NEXT: Qualifiers:{{ *}}[ ptr ]