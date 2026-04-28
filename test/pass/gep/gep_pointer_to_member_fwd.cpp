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

// Use a pointer to member variable (A::*ptr)
double* foo(A* ar, B A::*member_pointer) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "double",
  (ar->*member_pointer).x = static_cast<double*>(malloc(sizeof(double)));
  return nullptr;
}
