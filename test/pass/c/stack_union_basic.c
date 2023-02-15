// RUN: %c-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

union BasicUnion {
  int a;
  float b;
};

int foo() {
  union BasicUnion b_union;
  b_union.a = 0;
  return b_union.a;
}
