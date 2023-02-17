// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

struct BasicStruct {
  int a;
};

int foo() {
  struct BasicStruct b_struct[10] = {0};
  return b_struct[0].a;
}
