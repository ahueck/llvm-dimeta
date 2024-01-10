// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

int foo() {
  const int a = 0;
  return a;
}

// CHECK: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   Qualifiers:      [ const ]
