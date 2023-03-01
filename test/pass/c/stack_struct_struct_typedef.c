// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

struct Inner {
  int a;
};

typedef struct Inner InnerS_t;

struct Outer {
  InnerS_t struct_inner;
};

int foo() {
  struct Outer out_struct = {0};
  return out_struct.struct_inner.a;
}
