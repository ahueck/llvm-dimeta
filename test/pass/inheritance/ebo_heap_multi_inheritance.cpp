// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %cpp-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

// #include <cstdlib>

extern "C" {
void* malloc(int);
}

struct Base {
  static double s_mem;
  static double s_mem_2;
  static double s_mem_3;
  int get() {
    return 1;
  }
};

struct BaseTwo {
  double s_mem;
};

struct Derived : public Base, BaseTwo {
  double pad;
  int* c;
};

void foo(Derived* d) {
  d->c = (int*)malloc(sizeof(int));
}

// CHECK: Line:            31
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   Qualifiers:      [ ptr ]
