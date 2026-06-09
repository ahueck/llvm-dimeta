// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// Fails, due to simply passing pointer s to malloc: %c-to-llvm %s | %opt -O2 -S | %apply-verifier 2>&1 | %filecheck %s

extern int hipMalloc(void** devPtr, int size);

typedef struct {
  int* data;
} MyStruct;

void foo(int n, MyStruct* s) {
  // clang-format off
  // CHECK: Type for heap-like: {{.*}}@hipMalloc
  // CHECK-NEXT: Extracted Type: {{.*}} = !DIDerivedType({{(tag: DW_TAG_member, name: "data")|(tag: DW_TAG_pointer_type)}}
  // CHECK-NEXT: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK: Location: "{{.*}}":"foo":[[@LINE+2]]
  // clang-format on
  hipMalloc((void**)&s->data, sizeof(int) * n);
}
