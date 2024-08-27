// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

typedef struct s3_t {
  int a[3];           // 0
  long b[2];          // 16
  char c;             // 32
  unsigned int d[3];  // 36
  char e[5];          // 48
  unsigned long f;    // 56
} s3;

int main(void) {
  s3 c;
  return 0;
}

// CHECK: - Name:            a
// CHECK-NEXT:  Builtin:         true
// CHECK-NEXT:  Type:
// CHECK-NEXT:    Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:    Array:           3