// RUN: %clang-cc -g -O2 -S -emit-llvm %s -o - | %apply-verifier -S 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %apply-verifier -S 2>&1 | %filecheck %s

#include <stdlib.h>

typedef struct {
  int nvars;
  int* vartypes;
} struct_grid;

#define taFree(ptr) (ptr != NULL ? (free((char*)(ptr)), ptr = NULL) : (ptr = NULL))
#define taMalloc(type, count) \
  ((unsigned int)(count) * sizeof(type)) > 0 ? ((type*)malloc((unsigned int)(sizeof(type) * (count)))) : (type*)NULL

void setVartypes(struct_grid* pgrid, int nvars, int* vartypes /* = i32 ptr */) {
  int* new_vartypes;
  taFree(pgrid->vartypes);
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  new_vartypes = taMalloc(int, nvars);  // llvm does not use bitcast (with -O1 and higher)
  for (int i = 0; i < nvars; i++) {
    new_vartypes[i] = vartypes[i];  // this is a memcpy (with -O1 and higher)
  }
  pgrid->nvars    = nvars;
  pgrid->vartypes = new_vartypes;
}