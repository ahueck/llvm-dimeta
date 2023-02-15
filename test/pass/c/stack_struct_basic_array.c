// RUN: %c-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

struct BasicStruct {
  int a[13];
};

int foo() {
  struct BasicStruct b_struct = {0};
  return b_struct.a[0];
}
