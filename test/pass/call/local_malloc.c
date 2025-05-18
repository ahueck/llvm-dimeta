// RUN: %c-to-llvm %s | %apply-verifier -kernel-call 2>&1 | %filecheck %s

#include "stdlib.h"

typedef struct Domain_t {
  float stoptime;
  float time;
  char c;
} Domain;

void kernel(float, Domain*);

void test() {
  Domain* d = (Domain*)malloc(sizeof(Domain));
  // CHECK: Final value type:
  // CHECK: Fundamental:     { Name: float, Extent: 4, Encoding: float }

  // CHECK: Final value type:
  // CHECK: Compound:
  // CHECK-NEXT:   Name:            Domain_t
  // CHECK-NEXT:   Type:            struct
  // CHECK-NEXT:   Extent:          12
  kernel(d->time, d);
}
