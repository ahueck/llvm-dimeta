// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

struct A {
  float b;
  float c;
  int* a[2];
};

void foo(struct A* ar) {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  ar->a[1] = (int*)malloc(sizeof(int));
}
