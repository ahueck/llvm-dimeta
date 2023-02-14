// RUN: %cpp-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

class X {
 public:
  int val;
};

int foo() {
  X class_x;
  return class_x.val;
}
