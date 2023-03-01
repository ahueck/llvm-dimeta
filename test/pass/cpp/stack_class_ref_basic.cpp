// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

class X {
 public:
  int& val;
};

int foo(int n) {
  X class_x{n};
  return class_x.val;
}
