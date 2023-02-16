// RUN: %cpp-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

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

  double foo() {
    return y;
  }
};
int foo() {
  Base b;
  X class_x;
  return class_x.y;
}
