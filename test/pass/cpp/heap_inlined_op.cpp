
// RUN: %cpp-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

extern "C" {
void* malloc(int);
}

struct ArrayWrapper {
  int** data;

  inline __attribute__((always_inline)) int** operator[](int index) {
    return &data[index];
  }
};

void foo(ArrayWrapper& wrapper) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK-NEXT: Pointer level: 1 (T*)
  *wrapper[1] = (int*)malloc(sizeof(int));
}
