// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %cpp-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

void foo(int** p) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK-NEXT: Pointer level: 1
  *p = new int[4];
}

void bar(double** o, int** p) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK-NEXT: Pointer level: 1
  *p = new int[2];
}
