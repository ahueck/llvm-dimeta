// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// clang-format off
// CHECK:    Name:            X
// CHECK-NEXT:    Identifier:      _ZTS1X
// CHECK-NEXT:    Type:            class
// CHECK-NEXT:    Extent:          16
// CHECK-NEXT:    Sizes:           [ 8, 4 ]
// CHECK-NEXT:    Offsets:         [ 0, 8 ]
// CHECK-NEXT:    Members:
// CHECK-NEXT:      - Name:            inner
// CHECK-NEXT:        Builtin:         false
// CHECK-NEXT:        Type:
// CHECK-NEXT:          Compound:
// CHECK-NEXT:            Name:            Inner
// CHECK-NEXT:            Identifier:      _ZTS5Inner
// CHECK-NEXT:            Type:            class
// CHECK-NEXT:            Extent:          8
// CHECK-NEXT:            Sizes:           [ 8 ]
// CHECK-NEXT:            Offsets:         [ 0 ]
// CHECK-NEXT:            Members:
// CHECK-NEXT:              - Name:            x
// CHECK-NEXT:                Builtin:         true
// CHECK-NEXT:                Type:
// CHECK-NEXT:                  Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:      - Name:            y
// CHECK-NEXT:        Builtin:         true
// CHECK-NEXT:        Type:
// CHECK-NEXT:          Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// clang-format on

class Inner {
 public:
  double x;
};

class X {
 public:
  Inner inner;
  int y;
};

double foo() {
  X class_x{};
  return class_x.inner.x;
}
