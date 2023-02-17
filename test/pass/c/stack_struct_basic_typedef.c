// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

typedef int Integer;

struct BasicStruct {
  Integer a;
};

int foo() {
  struct BasicStruct b_struct = {0};
  return b_struct.a;
}
