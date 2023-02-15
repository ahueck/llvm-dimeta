// RUN: %c-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

enum Color { RED, GREEN, BLUE };

enum Color foo() {
  enum Color color = GREEN;
  return color;
}
