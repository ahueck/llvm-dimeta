// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

class X {
 public:
  int& val;
};

int foo(int n) {
  X class_x{n};
  return class_x.val;
}

// CHECK:   Line:            9
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            X
// CHECK-NEXT:     Identifier:      _ZTS1X
// CHECK-NEXT:     Type:            class
// CHECK-NEXT:     Extent:          8
// CHECK-NEXT:     Sizes:           [ 8 ]
// CHECK-NEXT:     Offsets:         [ 0 ]
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            val
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:           Qualifiers:      [ ref ]
