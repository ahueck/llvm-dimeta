// RUN: %cpp-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

class Base {
 public:
  double x;

  virtual double foo(){
    return x;
  }
};

class X : public Base {
 public:
  int y;

  virtual int bar() = 0;
};

class Y : public X {
 public:
  float z;

  int bar(){
    return y;
  }

  double foo(){
    return z;
  }
};

int foo() {
  Y class_y;
  return class_y.y;
}
