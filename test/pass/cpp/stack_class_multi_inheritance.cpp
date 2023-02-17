// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

class Base {
 public:
  double x;
};

class X {
 public:
  int y;
};

class Y : public X, public Base {
 public:
  float z;
};

int foo() {
  Y class_y;
  return class_y.y;
}
