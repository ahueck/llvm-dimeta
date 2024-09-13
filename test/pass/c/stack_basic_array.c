// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

int foo() {
  const int a[10] = {0};
  return a[1];
}

// CHECK: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   Array:           [ 10 ]
// CHECK-NEXT:   Qualifiers:      [ array, const ]
