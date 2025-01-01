// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// CHECK-NEXT: Pointer level: 1

struct A {
  int* a;
};

void foo() {
  struct A a_struct;
  a_struct.a = new int;
}
