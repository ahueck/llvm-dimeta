// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include "stdlib.h"

// Needs TBAA analysis for clang 10,14,15

typedef struct foo {
  double* data_;
} foo;

void take_field(foo* chunky2) {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
  chunky2->data_ = (double*)malloc(sizeof(double));
}