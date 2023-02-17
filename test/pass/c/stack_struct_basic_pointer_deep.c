// RUN: %c-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

struct BasicStruct {
  const int* a;   // read-only a[]
  int* const b;   // can write
  const int** c;  // read-only c[][]
};

int foo() {
  struct BasicStruct b_struct = {0};

  return b_struct.b[0] = 10;
}
