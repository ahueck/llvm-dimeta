// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

union BasicUnion {
  int a;
  float b;
};

int foo() {
  union BasicUnion b_union;
  b_union.a = 0;
  return b_union.a;
}

// CHECK: SourceLoc:
// CHECK-NEXT:   File:            {{.*}}stack_union_basic.c{{'?}}
// CHECK-NEXT:   Function:        foo
// CHECK-NEXT:   Line:            9
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            BasicUnion
// CHECK-NEXT:     Type:            union
// CHECK-NEXT:     Extent:          4
// CHECK-NEXT:     Sizes:           [ 4, 4 ]
// CHECK-NEXT:     Offsets:         [ 0, 0 ]
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            a
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:       - Name:            b
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: float, Extent: 4, Encoding: float }
