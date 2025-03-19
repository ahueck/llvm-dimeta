// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

class Base {
 public:
  double x;

  virtual double foo() {
    return x;
  }
};

class X : public Base {
 public:
  int y;

  virtual int bar() = 0;
};

class Y : public X {
 public:
  alignas(8) float z;

  int bar() {
    return y;
  }

  double foo() {
    return z;
  }
};

int foo() {
  Y class_y;
  return class_y.y;
}

// clang-format off
// CHECK:   Function:        foo
// CHECK-NEXT:   Line:            35
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            Y
// CHECK-NEXT:     Identifier:      _ZTS1Y
// CHECK-NEXT:     Type:            class
// CHECK-NEXT:     Extent:          32
// CHECK-NEXT:     Sizes:           [ 4 ]
// CHECK-NEXT:     Offsets:         [ 24 ]
// CHECK-NEXT:     Base:
// CHECK-NEXT:       - BaseClass:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            X
// CHECK-NEXT:             Identifier:      _ZTS1X
// CHECK-NEXT:             Type:            class
// CHECK-NEXT:             Extent:          24
// CHECK-NEXT:             Sizes:           [ 4 ]
// CHECK-NEXT:             Offsets:         [ 16 ]
// CHECK-NEXT:             Base:
// CHECK-NEXT:               - BaseClass:
// CHECK-NEXT:                   Compound:
// CHECK-NEXT:                     Name:            Base
// CHECK-NEXT:                     Identifier:      _ZTS4Base
// CHECK-NEXT:                     Type:            class
// CHECK-NEXT:                     Extent:          16
// CHECK-NEXT:                     Sizes:           [ 8, 8 ]
// CHECK-NEXT:                     Offsets:         [ 0, 8 ]
// CHECK-NEXT:                     Members:
// CHECK-NEXT:                       - Name:            '_vptr$Base'
// CHECK-NEXT:                         Builtin:         true
// CHECK-NEXT:                         Type:
// CHECK-NEXT:                           Fundamental:     { Name: __vtbl_ptr_type, Extent: 8,
// CHECK-NEXT:                                              Encoding: vtable_ptr }
// CHECK-NEXT:                           Qualifiers:      [ ptr, ptr ]
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
//          0 |   (Base vtable pointer)
//          8 |   double x
//            | [sizeof=16, dsize=16, align=8,
//            |  nvsize=16, nvalign=8]
// *** Dumping AST Record Layout
//          0 | class X
//          0 |   class Base (primary base)
//          0 |     (Base vtable pointer)
//          8 |     double x
//         16 |   int y
//            | [sizeof=24, dsize=20, align=8,
//            |  nvsize=20, nvalign=8]
// *** Dumping AST Record Layout
//          0 | class Y
//          0 |   class X (primary base)
//          0 |     class Base (primary base)
//          0 |       (Base vtable pointer)
//          8 |       double x
//         16 |     int y
//         24 |   float z
//            | [sizeof=32, dsize=28, align=8,
//            |  nvsize=28, nvalign=8]
