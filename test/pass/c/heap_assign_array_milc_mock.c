// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

#include "stdlib.h"
typedef struct {
  double c[3];
} su3_vector;

void take(void*);
void eo_fermion_force(int sites_on_node) {
  int mu, nu, rho, sig;
  su3_vector* tempvec[8];
  // su3_vector* temp_x;

  /* Allocate temporary vectors */
  for (mu = 0; mu < 8; mu++)
    tempvec[mu] = (su3_vector*)malloc(sites_on_node * sizeof(su3_vector));

  take((void*)tempvec[0]);
}

// CHECK:  Line:            17
// CHECK-NEXT:Builtin:         false
// CHECK-NEXT:Type:
// CHECK-NEXT:  Compound:
// CHECK-NEXT:    Name:            ''
// CHECK-NEXT:    Type:            struct
// CHECK-NEXT:    Extent:          24
// CHECK-NEXT:    Sizes:           [ 24 ]
// CHECK-NEXT:    Offsets:         [ 0 ]
// CHECK-NEXT:    Members:
// CHECK-NEXT:      - Name:            c
// CHECK-NEXT:        Builtin:         true
// CHECK-NEXT:        Type:
// CHECK-NEXT:          Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:          Array:           [ 3 ]
// CHECK-NEXT:          Qualifiers:      [ array ]
// CHECK-NEXT:  Qualifiers:      [ ptr ]
// CHECK-NEXT:  Typedef:         su3_vector