// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %cpp-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

void foo(int** p, int n) {
  *p = new int[n];
}
