// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

class Base {
 public:
  double x;
};

class X : public Base {
 public:
  int y;
};

class Y : public X {
 public:
  float z;
};

int foo() {
  Y class_y;
  return class_y.y;
}

// CHECK: Line:            21
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            Y
// CHECK-NEXT:     Identifier:      _ZTS1Y
// CHECK-NEXT:     Type:            class
// CHECK-NEXT:     Extent:          16
// CHECK-NEXT:     Sizes:           [ 4 ]
// CHECK-NEXT:     Offsets:         [ 12 ]
// CHECK-NEXT:     Base:
// CHECK-NEXT:       - BaseClass:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            X
// CHECK-NEXT:             Identifier:      _ZTS1X
// CHECK-NEXT:             Type:            class
// CHECK-NEXT:             Extent:          16
// CHECK-NEXT:             Sizes:           [ 4 ]
// CHECK-NEXT:             Offsets:         [ 8 ]
// CHECK-NEXT:             Base:
// CHECK-NEXT:               - BaseClass:
// CHECK-NEXT:                   Compound:
// CHECK-NEXT:                     Name:            Base
// CHECK-NEXT:                     Identifier:      _ZTS4Base
// CHECK-NEXT:                     Type:            class
// CHECK-NEXT:                     Extent:          8
// CHECK-NEXT:                     Sizes:           [ 8 ]
// CHECK-NEXT:                     Offsets:         [ 0 ]
// CHECK-NEXT:                     Members:
// CHECK-NEXT:                       - Name:            x
// CHECK-NEXT:                         Builtin:         true
// CHECK-NEXT:                         Type:
// CHECK-NEXT:                           Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:             Members:
// CHECK-NEXT:               - Name:            y
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            z
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: float, Extent: 4, Encoding: float }

// *** Dumping AST Record Layout
//          0 | class Base
//          0 |   double x
//            | [sizeof=8, dsize=8, align=8,
//            |  nvsize=8, nvalign=8]
// *** Dumping AST Record Layout
//          0 | class X
//          0 |   class Base (base)
//          0 |     double x
//          8 |   int y
//            | [sizeof=16, dsize=12, align=8,
//            |  nvsize=12, nvalign=8]
// *** Dumping AST Record Layout
//          0 | class Y
//          0 |   class X (base)
//          0 |     class Base (base)
//          0 |       double x
//          8 |     int y
//         12 |   float z
//            | [sizeof=16, dsize=16, align=8,
//            |  nvsize=16, nvalign=8]
