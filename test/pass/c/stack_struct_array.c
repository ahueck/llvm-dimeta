// RUN: %c-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

struct Inner {
  int a;
};

struct Outer {
  struct Inner struct_inner[11];
};

int foo() {
  struct Outer out_struct = {0};
  return out_struct.struct_inner[0].a;
}
