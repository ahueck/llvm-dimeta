// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

struct BasicStruct {
  int a;
};

int foo() {
  struct BasicStruct b_struct[13] = {0};
  return b_struct[0].a;
}

// CHECK: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            BasicStruct
// CHECK-NEXT:     Type:            struct
// CHECK-NEXT:     Extent:          4
// CHECK-NEXT:     Sizes:           [ 4 ]
// CHECK-NEXT:     Offsets:         [ 0 ]
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            a
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   Array:           13
