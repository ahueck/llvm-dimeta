// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: No located dimeta type.

void bar(const void*);

void foo() {
  bar((const void*)"Hello world");
}