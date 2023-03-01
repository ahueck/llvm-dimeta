// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

typedef int Integer;

int foo() {
  Integer a = 0;
  return a;
}
