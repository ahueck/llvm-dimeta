// RUN: %c-to-llvm %s | %apply-verifier -callbase-mode=argument-backward -callbase-arg=0 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier -callbase-mode=argument-backward -callbase-arg=0 2>&1 | %filecheck %s

extern int custom_cuda_alloc(void** devPtr, int size);

int* d_p;

void foo(int n) {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK: Location: "{{.*}}":"foo":[[@LINE+1]]
  custom_cuda_alloc((void**)&d_p, sizeof(int) * n);
}
