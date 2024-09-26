// RUN: %cpp-to-llvm %s | %apply-verifier | %filecheck %s
// RUN: %cpp-to-llvm %S/lulesh-init-mock.cpp | %apply-verifier --yaml-retained | %filecheck %s --check-prefix=NOFWD

#include "lulesh-mock.h"

int main(int argc, char* argv[]) {
  Domain* dom;

  dom = new Domain(argc, 1.2);

  return dom->getRanks();
}
// FIXME: Clang-12, unlike 14, generates full definition in dbg data:
// CHECK:   Line:            7
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:    Compound:
// CHECK-NEXT:      Name:            Domain
// CHECK-NEXT:      Identifier:      _ZTS6Domain
// CHECK-NEXT:      Type:            class
// CHECK-NEXT:      Extent:          16
// CHECK-NEXT:    Qualifiers:      [ ptr ]
// CHECK-NEXT:    ForwardDecl:     true

// NOFWD:  CUName:          '{{.*}}lulesh-init-mock.cpp'
// NOFWD-NEXT:  Types:
// NOFWD-NEXT:    - Builtin:         false
// NOFWD-NEXT:      Type:
// NOFWD-NEXT:        Compound:
// NOFWD-NEXT:          Name:            Domain
// NOFWD-NEXT:          Identifier:      _ZTS6Domain
// NOFWD-NEXT:          Type:            class
// NOFWD-NEXT:          Extent:          16
// NOFWD-NEXT:          Sizes:           [ 4, 8 ]
// NOFWD-NEXT:          Offsets:         [ 0, 8 ]
// NOFWD-NEXT:          Members:
// NOFWD-NEXT:            - Name:            m_ranks
// NOFWD-NEXT:              Builtin:         true
// NOFWD-NEXT:              Type:
// NOFWD-NEXT:                Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// NOFWD-NEXT:            - Name:            m_other
// NOFWD-NEXT:              Builtin:         true
// NOFWD-NEXT:              Type:
// NOFWD-NEXT:                Fundamental:     { Name: double, Extent: 8, Encoding: float }
