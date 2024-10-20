// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

class Base {
 public:
  double x;
};

class X {
 public:
  int y;
};

class Y : public X, public Base {
 public:
  float z;
};

int foo() {
  Y class_y;
  return class_y.y;
}

// CHECK:   Line:            19
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            Y
// CHECK-NEXT:     Identifier:      _ZTS1Y
// CHECK-NEXT:     Type:            class
// CHECK-NEXT:     Extent:          24
// CHECK-NEXT:     Sizes:           [ 4, 8, 4 ]
// CHECK-NEXT:     Offsets:         [ 0, 8, 16 ]
// CHECK-NEXT:     Base:
// CHECK-NEXT:       - BaseClass:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            X
// CHECK-NEXT:             Identifier:      _ZTS1X
// CHECK-NEXT:             Type:            class
// CHECK-NEXT:             Extent:          4
// CHECK-NEXT:             Sizes:           [ 4 ]
// CHECK-NEXT:             Offsets:         [ 0 ]
// CHECK-NEXT:             Members:
// CHECK-NEXT:               - Name:            y
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:       - BaseClass:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            Base
// CHECK-NEXT:             Identifier:      _ZTS4Base
// CHECK-NEXT:             Type:            class
// CHECK-NEXT:             Extent:          8
// CHECK-NEXT:             Sizes:           [ 8 ]
// CHECK-NEXT:             Offsets:         [ 0 ]
// CHECK-NEXT:             Members:
// CHECK-NEXT:               - Name:            x
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            z
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: float, Extent: 4, Encoding: float }
