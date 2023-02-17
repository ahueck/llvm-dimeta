// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

int foo() {
  const int a[10] = {0};
  return a[1];
}
