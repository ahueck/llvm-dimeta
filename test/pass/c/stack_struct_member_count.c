
// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

typedef struct s4_t {
  double b[3];  // 0
  double c[4];  // 24
} s4;

int main(void) {
  s4 d;
  return 0;
}