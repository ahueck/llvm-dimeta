// RUN: %cpp-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

class Base {
 public:
  double x;
};

class X : public Base {
 public:
  int y;
};

class Y : public X {
 public:
  float z;
};

int foo() {
  Y class_y;
  return class_y.y;
}
