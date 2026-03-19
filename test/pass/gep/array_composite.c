// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

struct B {};

struct A {
  float b;
  float c;
  struct B* a[2];
};

void foo(struct A* ar) {
  ar->a[1] = (struct B*)malloc(sizeof(struct B));
}

// CHECK: SourceLoc:
// CHECK:   Line:            15
// CHECK: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            B
// CHECK-NEXT:     Type:            struct
// CHECK-NEXT:     Extent:          0
// CHECK-NEXT:   Qualifiers:      [ ptr ]