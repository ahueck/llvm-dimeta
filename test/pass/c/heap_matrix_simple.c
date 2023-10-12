// RUN: %clang-cc -g -O2 -S -emit-llvm %s -o - | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

#define hypre_TAlloc(type, count) ((type*)malloc((unsigned int)(sizeof(type) * (count))))

void matrix(float**** symmetric, int part, int nvars, int nparts, int vi, int vj) {
  // CHECK: Extracted Type: !3 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK: Extracted Type: !4 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !5, size: 64)
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK: Extracted Type: !5 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
  // CHECK: Final Type: !6 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  *symmetric               = hypre_TAlloc(float**, nparts);
  *symmetric[part]         = hypre_TAlloc(float*, nvars);
  *symmetric[part][vi]     = hypre_TAlloc(float, nvars);
  *symmetric[part][vi][vj] = 0.0f;
}
