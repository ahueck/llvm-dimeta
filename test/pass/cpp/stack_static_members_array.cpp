// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

struct S {
  static int a[];
};

void bar() {
  S struct_variable;
}

int S::a[10];  // def, complete type

// CHECK:       - Name:            a
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:           Array:           [ 0 ]
// CHECK-NEXT:           Qualifiers:      [ array, static ]
