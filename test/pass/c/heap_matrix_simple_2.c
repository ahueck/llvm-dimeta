// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

#define hypre_TAlloc(type, count) ((type*)malloc((unsigned int)(sizeof(type) * (count))))

void matrix(int part, int nvars, int nparts, int vi, int vj) {
  // clang-format off
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[PTRLVL1:![0-9]+]], size: 64)
  // CHECK: Final Type: [[BASET:![0-9]+]] = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK-NEXT: Pointer level: 3
  // CHECK: Extracted Type: [[PTRLVL1]] = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[PTRLVL2:![0-9]+]], size: 64)
  // CHECK: Final Type: [[BASET]] = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK-NEXT: Pointer level: 2
  // CHECK: Extracted Type: [[PTRLVL2]] = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[BASET]], size: 64)
  // CHECK: Final Type: [[BASET]] = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK-NEXT: Pointer level: 1
  // clang-format on
  float*** symmetric;
  symmetric               = hypre_TAlloc(float**, nparts);  // (float***)malloc
  symmetric[part]         = hypre_TAlloc(float*, nvars);    // (float**)malloc
  symmetric[part][vi]     = hypre_TAlloc(float, nvars);     // (float*)malloc
  symmetric[part][vi][vj] = 0.0f;
}
