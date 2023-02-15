// RUN: %c-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

struct BasicStruct {
  int a;
  float b;
  double c;
};

int foo() {
  struct BasicStruct b_struct = {0};
  return b_struct.a;
}
