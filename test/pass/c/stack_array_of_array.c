// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

// TODO: track subranges of arrays?

int foo() {
  const int a[10][8] = {0};
  return a[1][0];
}

// CHECK: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   Array:           80
// CHECK-NEXT:   Qualifiers:      [ const ]
