// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Name:            X
// CHECK-NEXT: Identifier:      _ZTS1X
// CHECK-NEXT: Type:            class
// CHECK-NEXT: Extent:          4
// CHECK-NEXT: Sizes:           [ 4 ]
// CHECK-NEXT: Offsets:         [ 0 ]
// CHECK-NEXT: Members:
// CHECK-NEXT: - Name:            val
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT: Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }

class X {
 public:
  int val;
};

int foo() {
  X class_x;
  return class_x.val;
}
