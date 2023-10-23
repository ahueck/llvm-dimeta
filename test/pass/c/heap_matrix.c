// RUN: %clang-cc -g -O2 -S -emit-llvm %s -o - | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

#define hypre_TAlloc(type, count) \
  ((unsigned int)(count) * sizeof(type)) > 0 ? ((type*)malloc((unsigned int)(sizeof(type) * (count)))) : (type*)NULL

void matrix(float**** symmetric, int part, int vi, int vj, int nparts, int nvars) {
  // clang-format off
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[PTRLVL1:![0-9]+]], size: 64)
  // CHECK: Final Type: [[BASET:![0-9]+]] = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK: Extracted Type: [[PTRLVL1]] = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[PTRLVL2:![0-9]+]], size: 64)
  // CHECK: Final Type: [[BASET]] = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK: Extracted Type: [[PTRLVL2]] = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[BASET]], size: 64)
  // CHECK: Final Type: [[BASET]] = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // clang-format on
  *symmetric               = hypre_TAlloc(float**, nparts);
  *symmetric[part]         = hypre_TAlloc(float*, nvars);
  *symmetric[part][vi]     = hypre_TAlloc(float, nvars);
  *symmetric[part][vi][vj] = 0.0f;
}