// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

void foo(int rows, int cols) {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  int** a = new int*[rows];
  for (int i = 0; i < rows; ++i) {
    // CHECK: Extracted Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
    // CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
    a[i] = new int[cols];
  }
}
