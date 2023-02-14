// RUN: %cpp-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

class X {
 public:
  int& val;
};

int foo(int n) {
  X class_x{n};
  return class_x.val;
}
