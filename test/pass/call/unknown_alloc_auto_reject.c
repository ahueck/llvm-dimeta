// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

extern void* custom_alloc(int size);

int* p;

void foo(int n) {
  p = (int*)custom_alloc(sizeof(int) * n);
}

// CHECK: Function: foo:
// CHECK-NOT: Type for heap-like: {{.*}}@custom_alloc
