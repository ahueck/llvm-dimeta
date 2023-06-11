// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

// XFAIL: *

#include <stdlib.h>

struct B {
  int* a;
};
struct A {
  struct B b;
};

void foo() {
  struct A a_struct;
  a_struct.b.a = malloc(sizeof(int));
}
