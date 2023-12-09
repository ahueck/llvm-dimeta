// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// CHECK-NEXT: Pointer level: 1

void foo(int n) {
  int* p = new int[n];
}
