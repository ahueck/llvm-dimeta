// RUN: %c-to-llvm %s | %apply-verifier -callbase-mode=return-flow 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier -callbase-mode=return-flow 2>&1 | %filecheck %s

extern void* custom_alloc(int size);

int* p;

void foo(int n) {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK: Location: "{{.*}}":"foo":[[@LINE+1]]
  p = (int*)custom_alloc(sizeof(int) * n);
}
