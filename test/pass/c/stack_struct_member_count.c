
// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

typedef struct s4_t {
  double b[3];  // 0
  double c[4];  // 24
} s4;

int main(void) {
  s4 d;
  return 0;
}

// CHECK:   Compound:
// CHECK-NEXT:     Name:            s4_t
// CHECK-NEXT:     Type:            struct
// CHECK-NEXT:     Extent:          56
// CHECK-NEXT:     Sizes:           [ 24, 32 ]
// CHECK-NEXT:     Offsets:         [ 0, 24 ]
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            b
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:           Array:           3
// CHECK-NEXT:           Qualifiers:      [ array ]
// CHECK-NEXT:       - Name:            c
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:           Array:           4
// CHECK-NEXT:           Qualifiers:      [ array ]
// CHECK-NEXT:   Typedef:         s4
