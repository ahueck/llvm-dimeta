// RUN: %c-to-llvm %s | %apply-verifier -S 2>&1 | %filecheck %s

typedef int int2 __attribute__((ext_vector_type(2)));
typedef float float2 __attribute__((ext_vector_type(2)));
typedef double double3 __attribute__((ext_vector_type(3)));

void alloc_vector_arrays() {
  int2 i[3];
  float2 f[3];
  double3 d[4];
}

// CHECK:   Line:            8
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   Array:           [ 3, 2 ]
// CHECK-NEXT:   Qualifiers:      [ array, vector ]
// CHECK-NEXT:   Typedef:         int2
// CHECK-NEXT:   Vector:          true
// CHECK-NEXT:   VectorSize:      8

// CHECK:   Line:            9
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: float, Extent: 4, Encoding: float }
// CHECK-NEXT:   Array:           [ 3, 2 ]
// CHECK-NEXT:   Qualifiers:      [ array, vector ]
// CHECK-NEXT:   Typedef:         float2
// CHECK-NEXT:   Vector:          true
// CHECK-NEXT:   VectorSize:      8

// CHECK:   Line:            10
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:   Array:           [ 4, 3 ]
// CHECK-NEXT:   Qualifiers:      [ array, vector ]
// CHECK-NEXT:   Typedef:         double3
// CHECK-NEXT:   Vector:          true
// CHECK-NEXT:   VectorSize:      32
