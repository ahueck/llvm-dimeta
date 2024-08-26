// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Could not determine type (missing entry type)

#include <stdlib.h>

extern double* a;

int main(int argc, char** argv) {
  int n = argc * 2;

  a = (double*)malloc(sizeof(double) * n);

  free(a);

  return 0;
}