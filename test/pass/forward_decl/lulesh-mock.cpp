// RUN: %cpp-to-llvm %s | %apply-verifier | %filecheck %s

// TODO: API needs to find retained type in DICompileUnit to determine Domain layout:
// %cpp-to-llvm %S/lulesh-init-mock.cpp | %apply-verifier 2>&1 | %filecheck %s --check-prefix=NOFWD

#include "lulesh-mock.h"

int main(int argc, char* argv[]) {
  Domain* dom;

  dom = new Domain(argc, 1.2);

  return dom->getRanks();
}

// CHECK:    Compound:
// CHECK-NEXT:      Name:            Domain
// CHECK-NEXT:      Identifier:      _ZTS6Domain
// CHECK-NEXT:      Type:            class
// CHECK-NEXT:      Extent:          16
// CHECK-NEXT:    ForwardDecl:     true
