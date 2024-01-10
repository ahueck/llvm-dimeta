// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

enum Color { RED, GREEN, BLUE };

enum Color foo() {
  enum Color color = GREEN;
  return color;
}

// CHECK: SourceLoc:
// CHECK-NEXT:   File:            {{.*}}stack_enum_basic.c{{'?}}
// CHECK-NEXT:   Function:        foo
// CHECK-NEXT:   Line:            6
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            Color
// CHECK-NEXT:     Type:            enum
// CHECK-NEXT:     Extent:          4
// CHECK-NEXT:     Sizes:           [ 4 ]
// CHECK-NEXT:     Offsets:         [ 0 ]
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            RED
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: unsigned int, Extent: 4, Encoding: unsigned_int }
// CHECK-NEXT:       - Name:            GREEN
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: unsigned int, Extent: 4, Encoding: unsigned_int }
// CHECK-NEXT:       - Name:            BLUE
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: unsigned int, Extent: 4, Encoding: unsigned_int }
