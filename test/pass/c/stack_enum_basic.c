// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

enum Color { RED, GREEN, BLUE };

enum Color foo() {
  enum Color color = GREEN;
  return color;
}
