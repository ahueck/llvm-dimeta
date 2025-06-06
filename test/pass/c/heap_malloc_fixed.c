// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

void foo() {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  int* p = malloc(sizeof(int) * 4);
}
