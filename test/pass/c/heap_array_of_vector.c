// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

typedef int int2 __attribute__((ext_vector_type(2)));
typedef float float2 __attribute__((ext_vector_type(2)));
typedef double double3 __attribute__((ext_vector_type(3)));

void malloc_vector(int n) {
  int2* i;    
  float2* f;  
  double3* d; 
  i= malloc(n * sizeof(int2));
  f = malloc(n * sizeof(float2));
  d = malloc(n * sizeof(double3));
}


// CHECK:   Line:            13
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   Array:           [ 2 ]
// CHECK-NEXT:   Qualifiers:      [ ptr, vector ]
// CHECK-NEXT:   Typedef:         int2
// CHECK-NEXT:   Vector:          true
// CHECK-NEXT:   VectorSize:      8

// CHECK:   Line:            14
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: float, Extent: 4, Encoding: float }
// CHECK-NEXT:   Array:           [ 2 ]
// CHECK-NEXT:   Qualifiers:      [ ptr, vector ]
// CHECK-NEXT:   Typedef:         float2
// CHECK-NEXT:   Vector:          true
// CHECK-NEXT:   VectorSize:      8

// CHECK:   Line:            15
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:   Array:           [ 3 ]
// CHECK-NEXT:   Qualifiers:      [ ptr, vector ]
// CHECK-NEXT:   Typedef:         double3
// CHECK-NEXT:   Vector:          true
// CHECK-NEXT:   VectorSize:      32