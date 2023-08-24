// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)

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

unsigned* foo(int n) {
  // CHECK: unsigned int
  unsigned* p = (unsigned*)malloc(sizeof(unsigned) * n);
  struct P struct_P;
  struct X s[10];
  return p;
}
