// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

class X {};

class Y : X {
 public:
  bool bool_m;
};

void foo() {
  Y y_var;
}

// CHECK: Line:            13
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            Y
// CHECK-NEXT:     Identifier:      _ZTS1Y
// CHECK-NEXT:     Type:            class
// CHECK-NEXT:     Extent:          1
// CHECK-NEXT:     Sizes:           [ 1 ]
// CHECK-NEXT:     Offsets:         [ 0 ]
// CHECK-NEXT:     Base:
// CHECK-NEXT:       - BaseClass:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            X
// CHECK-NEXT:             Identifier:      _ZTS1X
// CHECK-NEXT:             Type:            class
// CHECK-NEXT:             Extent:          1
// CHECK-NEXT:         EBO:             true
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            bool_m
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: bool, Extent: 1, Encoding: bool }

// *** Dumping AST Record Layout
//          0 | class X (empty)
//            | [sizeof=1, dsize=1, align=1,
//            |  nvsize=1, nvalign=1]
// *** Dumping AST Record Layout
//          0 | class Y
//          0 |   class X (base) (empty)
//          0 |   _Bool bool_m
//            | [sizeof=1, dsize=1, align=1,
//            |  nvsize=1, nvalign=1]
