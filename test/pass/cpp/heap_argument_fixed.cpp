// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

void foo(int** p) {
  *p = new int[4];
}
