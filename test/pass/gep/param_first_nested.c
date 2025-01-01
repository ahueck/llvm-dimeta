// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// the code becomes a store of malloc to chunky (no gep etc.) -> need to rely on TBAA:
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include "stdlib.h"

// Needs TBAA analysis for clang 10,14,15

typedef struct foo {
  struct {
    struct xx {
      struct bb {
        double* x;
        double* z;
      } baz;
    } foobar;
  } bar;

} foo;

void take_field(foo* chunky2) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
  chunky2->bar.foobar.baz.x = (double*)malloc(sizeof(double));
}