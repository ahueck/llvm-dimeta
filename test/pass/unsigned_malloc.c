// RUN: %c-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

#include <stdlib.h>

double global;

struct P {
  float float_P;
  const char* char_P;
};

struct X {
  unsigned unsigned_X;
  struct P struct_X;
};

void foo(int n) {
  // CHECK: unsigned int
  unsigned* p = malloc(sizeof(unsigned) * n);
  struct P struct_P;
  struct X s[10];
}
