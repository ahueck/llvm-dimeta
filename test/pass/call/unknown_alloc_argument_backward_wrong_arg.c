// RUN: %c-to-llvm %s | %apply-verifier -callbase-mode=argument-backward -callbase-arg=1 2>&1 | %filecheck %s

extern int custom_cuda_alloc(void** devPtr, int size);

int* d_p;

void foo(int n) {
  custom_cuda_alloc((void**)&d_p, sizeof(int) * n);
}

// CHECK: Function: foo:
// CHECK-NOT: Type for heap-like: {{.*}}@custom_cuda_alloc
