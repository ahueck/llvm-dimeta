// RUN: %clang-cc -g -O2 -S -emit-llvm %s -o - | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

#define hypre_TAlloc(type, count) ((type*)malloc((unsigned int)(sizeof(type) * (count))))

void matrix(float**** symmetric, int part, int nvars, int nparts, int vi, int vj) {
  // clang-format off
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[PTRLVL1:![0-9]+]], size: 64)
  // CHECK: Final Type: [[BASET:![0-9]+]] = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK: Extracted Type: [[PTRLVL1]] = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[PTRLVL2:![0-9]+]], size: 64)
  // CHECK: Final Type: [[BASET]] = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK: Extracted Type: [[PTRLVL2]] = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[BASET]], size: 64)
  // CHECK: Final Type: [[BASET]] = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // clang-format on
  *symmetric               = hypre_TAlloc(float**, nparts);  // (float***)malloc
  *symmetric[part]         = hypre_TAlloc(float*, nvars);    // (float**)malloc
  *symmetric[part][vi]     = hypre_TAlloc(float, nvars);     // (float*)malloc
  *symmetric[part][vi][vj] = 0.0f;
}
