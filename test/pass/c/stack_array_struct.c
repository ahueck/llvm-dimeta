// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

struct BasicStruct {
  int a;
};

int foo() {
  struct BasicStruct b_struct[10] = {0};
  return b_struct[0].a;
}

// CHECK: SourceLoc:
// CHECK-NEXT:   File:            {{.*}}stack_array_struct.c{{'?}}
// CHECK-NEXT:   Function:        foo
// CHECK-NEXT:   Line:            8
// CHECK-NEXT: Builtin:         false
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
// CHECK-NEXT:   Array:           [ 10 ]
