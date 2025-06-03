// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

double* a;

void foo(int argc) {
  a = (double*)malloc(sizeof(double) * argc);
}
// malloc:
// CHECK: Final Type: {{.*}} = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
// CHECK: Pointer level: 1
// CHECK: Location: "{{.*}}":"foo":9