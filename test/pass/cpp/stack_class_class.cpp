// RUN: %cpp-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

class Inner {
 public:
  double x;
};

class X {
 public:
  Inner inner;
  int y;
};

double foo() {
  X class_x{};
  return class_x.inner.x;
}
