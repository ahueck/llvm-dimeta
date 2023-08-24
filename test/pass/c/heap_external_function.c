// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

#include <stdlib.h>

extern void g(int* data);

void foo(int n) {
  g(malloc(sizeof(int)));
}

extern void h(double, double, double, float, int* data);

void bar() {
  double d = 0.0;
  h(d, d, d, d, malloc(sizeof(int)));
}