// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

enum class Color : char { red, green, blue };

Color foo() {
  Color color = Color::blue;
  return color;
}

// clang-format off
// CHECK:     Identifier:      _ZTS5Color
// CHECK-NEXT:     Type:            enum
// CHECK-NEXT:     Extent:          1
// CHECK-NEXT:     Sizes:           [ 1 ]
// CHECK-NEXT:     Offsets:         [ 0 ]
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            red
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT:       - Name:            green
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT:       - Name:            blue
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: char, Extent: 1, Encoding: signed_char }
