// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Could not determine type (missing entry type)

// REQUIRES: local

#include <stdlib.h>

extern double* a;

void foo(int argc) {
  a = (double*)malloc(sizeof(double) * argc);
}