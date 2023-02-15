// RUN: %c-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

int foo() {
  int a[10] = {0};
  return a[1];
}
