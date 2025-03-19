// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Name:            X
// CHECK-NEXT: Identifier:      _ZTS1X
// CHECK-NEXT: Type:            class
// CHECK-NEXT: Extent:          1
// CHECK-NEXT: Static_Members:
// CHECK-NEXT: - Name:            val
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT: Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT: Qualifiers:      [ ptr, static ]

class X {
 public:
  static int* val;
};

void foo() {
  X x;
}

// *** Dumping AST Record Layout
//          0 | class X (empty)
//            | [sizeof=1, dsize=1, align=1,
//            |  nvsize=1, nvalign=1]
