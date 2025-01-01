// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

struct MemberS {
  int x;
};
class Domain {
 public:
  MemberS m;
};

void foo() {
  MemberS Domain::*pointer_to = &Domain::m;
}

// CHECK:Builtin:         false
// CHECK-NEXT:Type:
// CHECK-NEXT:  Compound:
// CHECK-NEXT:    Name:            MemberS
// CHECK-NEXT:    Identifier:      _ZTS7MemberS
// CHECK-NEXT:    Type:            struct
// CHECK-NEXT:    Extent:          4
// CHECK-NEXT:    Sizes:           [ 4 ]
// CHECK-NEXT:    Offsets:         [ 0 ]
// CHECK-NEXT:    Members:
// CHECK-NEXT:      - Name:            x
// CHECK-NEXT:        Builtin:         true
// CHECK-NEXT:        Type:
// CHECK-NEXT:          Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:  Qualifiers:      [ ptr_to_mem ]
