// RUN: %c-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

struct BasicStruct {
  int a;
};

int foo() {
  struct BasicStruct b_struct[13] = {0};
  return b_struct[0].a;
}
