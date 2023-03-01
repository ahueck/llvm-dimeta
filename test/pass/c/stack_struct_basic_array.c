// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

struct BasicStruct {
  int a[13];
};

int foo() {
  struct BasicStruct b_struct = {0};
  return b_struct.a[0];
}
