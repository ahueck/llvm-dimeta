// RUN: %cpp-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

class Base {
 public:
  double x;
};

class X : public Base {
 public:
  int y;
};
int foo() {
  Base b;
  X class_x;
  return class_x.y;
}
