// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

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
