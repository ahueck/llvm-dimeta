// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

class Base {
 public:
  double x;

  virtual void foo(){};
};

class X {
 public:
  int y;

  virtual int bar() {
    return y;
  };
};

class Y : public X, public Base {
 public:
  float z;
};

int foo() {
  Y class_y;
  return class_y.y;
}
