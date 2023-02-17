// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

enum class Color : char { red, green, blue };

Color foo() {
  Color color = Color::blue;
  return color;
}
