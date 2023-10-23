// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

void foo(int rows, int cols) {
  // CHECK: Final Type: [[DIREF2:![0-9]+]] = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK-NEXT: Pointer level: 2
  int** a = new int*[rows];
  for (int i = 0; i < rows; ++i) {
    // CHECK: Final Type: [[DIREF2]] = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
    // CHECK-NEXT: Pointer level: 1
    a[i] = new int[cols];
  }
}
