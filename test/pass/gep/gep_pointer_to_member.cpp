// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

// XFAIL: *

// CHECK-NOT: Assertion

extern "C" {
void* malloc(int);
}

struct B {
  double* x;
};

struct A {
  float b;
  float c;
  B bs;
};

A k_astruct;
B k_bstruct;

// Use a pointer to member variable (A::*ptr)
void foo(A* ar, B A::*member_pointer) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "double",
  (ar->*member_pointer).x = static_cast<double*>(malloc(sizeof(double)));
}
