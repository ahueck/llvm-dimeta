// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

struct Foo;

struct S {
  static int a;
  static Foo x;
  static S s;
};

void bar() {
  S struct_variable;
}

// CHECK:       - Name:            a
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:           Qualifiers:      [ static ]
// CHECK-NEXT:       - Name:            x
// CHECK-NEXT:         Builtin:         false
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            Foo
// CHECK-NEXT:             Identifier:      _ZTS3Foo
// CHECK-NEXT:             Type:            struct
// CHECK-NEXT:             Extent:          0
// CHECK-NEXT:           Qualifiers:      [ static ]
// CHECK-NEXT:           ForwardDecl:     true
// CHECK-NEXT:       - Name:            s
// CHECK-NEXT:         Builtin:         false
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            S
// CHECK-NEXT:             Identifier:      _ZTS1S
// CHECK-NEXT:             Type:            struct
// CHECK-NEXT:             Extent:          1
// CHECK-NEXT:           Qualifiers:      [ static ]
// CHECK-NEXT:           Recurring:       true
