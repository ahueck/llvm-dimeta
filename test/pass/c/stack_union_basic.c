// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

union BasicUnion {
  int a;
  float b;
};

int foo() {
  union BasicUnion b_union;
  b_union.a = 0;
  return b_union.a;
}
