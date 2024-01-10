// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

struct BasicStruct {
  const int* a;   // read-only a[]
  int* const b;   // can write
  const int** c;  // read-only c[][]
};

int foo() {
  struct BasicStruct b_struct = {0};

  return b_struct.b[0] = 10;
}

// CHECK: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            BasicStruct
// CHECK-NEXT:     Type:            struct
// CHECK-NEXT:     Extent:          24
// CHECK-NEXT:     Sizes:           [ 8, 8, 8 ]
// CHECK-NEXT:     Offsets:         [ 0, 8, 16 ]
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            a
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:           Qualifiers:      [ ptr, const ]
// CHECK-NEXT:       - Name:            b
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:           Qualifiers:      [ const, ptr ]
// CHECK-NEXT:       - Name:            c
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:           Qualifiers:      [ ptr, ptr, const ]
