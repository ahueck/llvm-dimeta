// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

class Base {
 public:
  double x;

  virtual double foo() {
    return x;
  }
};

class X : public Base {
 public:
  int y;
  char c;
  unsigned char d;

  double foo() {
    return y;
  }
};
int foo() {
  Base b;
  X class_x;
  return class_x.y;
}
