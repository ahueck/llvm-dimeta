// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

class Base {
 private:
  double a;
  int b;
};

class Derived : public Base {
  int c;
};

void foo() {
  Derived d;
}

// CHECK: Line:            16
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            Derived
// CHECK-NEXT:     Identifier:      _ZTS7Derived
// CHECK-NEXT:     Type:            class
// CHECK-NEXT:     Extent:          16
// CHECK-NEXT:     Sizes:           [ 4 ]
// CHECK-NEXT:     Offsets:         [ 12 ]
// CHECK-NEXT:     Base:
// CHECK-NEXT:       - BaseClass:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            Base
// CHECK-NEXT:             Identifier:      _ZTS4Base
// CHECK-NEXT:             Type:            class
// CHECK-NEXT:             Extent:          16
// CHECK-NEXT:             Sizes:           [ 8, 4 ]
// CHECK-NEXT:             Offsets:         [ 0, 8 ]
// CHECK-NEXT:             Members:
// CHECK-NEXT:               - Name:            a
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:               - Name:            b
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            c
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }

// *** Dumping AST Record Layout
//          0 | class Base
//          0 |   double a
//          8 |   int b
//            | [sizeof=16, dsize=12, align=8,
//            |  nvsize=12, nvalign=8]
// *** Dumping AST Record Layout
//          0 | class Derived
//          0 |   class Base (base)
//          0 |     double a
//          8 |     int b
//         12 |   int c
//            | [sizeof=16, dsize=16, align=8,
//            |  nvsize=16, nvalign=8]
