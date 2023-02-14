// RUN: %cpp-to-llvm %s | %apply-dimeta 2>&1 | %filecheck %s

enum class Color : char { red, green, blue };

Color foo() {
  Color color = Color::blue;
  return color;
}
