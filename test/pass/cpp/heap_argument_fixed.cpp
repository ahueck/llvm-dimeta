// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

void foo(int** p) {
  *p = new int[4];
}

void bar(double** o, int** p) {
  *p = new int[2];
}
