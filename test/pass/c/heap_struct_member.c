// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

struct A {
  int* a;
};

void foo() {
  struct A a_struct;
  // CHECK: Final Type: !{{[0-9]+}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  a_struct.a = malloc(sizeof(int));
}
