// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

struct Inner {
  int a;
};

struct Outer {
  struct Inner struct_inner;
  double b;
};

int foo() {
  struct Outer out_struct = {0};
  return out_struct.struct_inner.a;
}
