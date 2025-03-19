// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

struct Base {};

struct Derived : public Base {
  int c;
};

void foo() {
  Derived d;
}

// CHECK: - BaseClass:
// CHECK-NEXT:     Compound:
// CHECK-NEXT:       Name:            Base
// CHECK-NEXT:       Identifier:      _ZTS4Base
// CHECK-NEXT:       Type:            struct
// CHECK-NEXT:       Extent:          1
// CHECK-NEXT:   EBO:             true