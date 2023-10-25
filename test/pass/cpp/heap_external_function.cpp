// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %cpp-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

extern void g(int* data);

void foo(int n) {
  g(new int[n]);
}

extern void h(double, double, double, float, int* data);

void bar(int n) {
  double d{0.0};
  h(d, d, d, d, new int[n]);
}