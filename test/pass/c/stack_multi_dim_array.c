// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

void test() {
  const int n = 16;
  const int m = 32;
  int a[n];
  int b[n][n];
  int c[n][m];
  int d[n][m][n];
}

// CHECK:  Array:           [ 16 ]
// CHECK:  Array:           [ 256 ]
// CHECK:  Array:           [ 512 ]
// CHECK:  Array:           [ 8192 ]