// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

struct A {
  int* a;
};

void foo() {
  struct A a_struct;
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  a_struct.a = malloc(sizeof(int));
}
