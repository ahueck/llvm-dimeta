// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// XFAIL: *

void foo(int n) {
  float vals[n];
}