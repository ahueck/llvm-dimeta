// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

class Base {
 public:
  double x;

  virtual double foo() {
    return x;
  }
};

class X : public Base {
 public:
  int* y;

  double foo() {
    return *y;
  }
};
int foo() {
  Base b;
  X class_x;
  return *class_x.y;
}

// clang-format off
// CHECK:   Line:            22
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            X
// CHECK-NEXT:     Identifier:      _ZTS1X
// CHECK-NEXT:     Type:            class
// CHECK-NEXT:     Extent:          24
// CHECK-NEXT:     Sizes:           [ 8 ]
// CHECK-NEXT:     Offsets:         [ 16 ]
// CHECK-NEXT:     Base:
// CHECK-NEXT:       - BaseClass:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            Base
// CHECK-NEXT:             Identifier:      _ZTS4Base
// CHECK-NEXT:             Type:            class
// CHECK-NEXT:             Extent:          16
// CHECK-NEXT:             Sizes:           [ 8, 8 ]
// CHECK-NEXT:             Offsets:         [ 0, 8 ]
// CHECK-NEXT:             Members:
// CHECK-NEXT:               - Name:            '_vptr$Base'
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: __vtbl_ptr_type, Extent: 8,
// CHECK-NEXT:                                      Encoding: vtable_ptr }
// CHECK-NEXT:                   Qualifiers:      [ ptr, ptr ]
// CHECK-NEXT:               - Name:            x
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            y
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:           Qualifiers:      [ ptr ]


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
//         16 |   int * y
//            | [sizeof=24, dsize=24, align=8,
//            |  nvsize=24, nvalign=8]
