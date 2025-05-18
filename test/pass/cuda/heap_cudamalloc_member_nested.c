// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O2 -S | %apply-verifier 2>&1 | %filecheck %s

extern int cudaMalloc(void** devPtr, int size);

struct B {
  double* x;
};

struct A {
  float b;
  float c;
  struct B* a;
};

void foo(struct A* ar) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
  cudaMalloc((void**)&ar->a->x, sizeof(double));
}