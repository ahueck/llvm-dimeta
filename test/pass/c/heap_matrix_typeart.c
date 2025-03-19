// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>
#include <string.h>
typedef struct mat_t {
  double* vals;
  int dim[2];
} mat;

int multiply(mat a, mat b, mat result) {
  int rows = a.dim[0];
  int cols = b.dim[1];

  int n = a.dim[1];

  if (n != b.dim[0] || result.dim[0] != rows || result.dim[1] != cols)
    return 0;

  int num_vals = rows * cols;
  double* temp = malloc(num_vals * sizeof(double));

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      double val = 0;
      for (int k = 0; k < n; k++) {
        val += a.vals[i * cols + k] * b.vals[k * cols + j];
      }
      temp[i * cols + j] = val;
    }
  }

  // memcpy(result.vals, temp, num_vals);
  free(temp);

  return 1;
}

// CHECK: Line:            20
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:   Qualifiers:      [ ptr ]