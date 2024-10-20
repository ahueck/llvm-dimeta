// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

class Base {
 public:
  double x;

  virtual void foo(){};
};

class X {
 public:
  int y;

  virtual int bar() {
    return y;
  };
};

class Y : public X, public Base {
 public:
  float z;
};

int foo() {
  Y class_y;
  return class_y.y;
}

// CHECK:   Line:            25
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            Y
// CHECK-NEXT:     Identifier:      _ZTS1Y
// CHECK-NEXT:     Type:            class
// CHECK-NEXT:     Extent:          40
// CHECK-NEXT:     Sizes:           [ 16, 16, 4 ]
// CHECK-NEXT:     Offsets:         [ 0, 16, 32 ]
// CHECK-NEXT:     Base:
// CHECK-NEXT:       - BaseClass:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            X
// CHECK-NEXT:             Identifier:      _ZTS1X
// CHECK-NEXT:             Type:            class
// CHECK-NEXT:             Extent:          16
// CHECK-NEXT:             Sizes:           [ 8, 4 ]
// CHECK-NEXT:             Offsets:         [ 0, 8 ]
// CHECK-NEXT:             Members:
// CHECK-NEXT:               - Name:            '_vptr$X'
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: __vtbl_ptr_type, Extent: 8,
// CHECK-NEXT:                                      Encoding: vtable_ptr }
// CHECK-NEXT:                   Qualifiers:      [ ptr, ptr ]
// CHECK-NEXT:               - Name:            y
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
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
// CHECK-NEXT:       - Name:            z
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: float, Extent: 4, Encoding: float }
