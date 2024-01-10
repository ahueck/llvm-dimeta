// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

struct Inner {
  int a;
};

typedef struct Inner InnerS_t;

struct Outer {
  InnerS_t struct_inner;
};

int foo() {
  struct Outer out_struct = {0};
  return out_struct.struct_inner.a;
}

// CHECK: SourceLoc:
// CHECK-NEXT:   File:            {{.*}}stack_struct_struct_typedef.c{{'?}}
// CHECK-NEXT:   Function:        foo
// CHECK-NEXT:   Line:            14
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            Outer
// CHECK-NEXT:     Type:            struct
// CHECK-NEXT:     Extent:          4
// CHECK-NEXT:     Sizes:           [ 4 ]
// CHECK-NEXT:     Offsets:         [ 0 ]
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            struct_inner
// CHECK-NEXT:         Builtin:         false
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            Inner
// CHECK-NEXT:             Type:            struct
// CHECK-NEXT:             Extent:          4
// CHECK-NEXT:             Sizes:           [ 4 ]
// CHECK-NEXT:             Offsets:         [ 0 ]
// CHECK-NEXT:             Members:
// CHECK-NEXT:               - Name:            a
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:           Typedef:         InnerS_t
