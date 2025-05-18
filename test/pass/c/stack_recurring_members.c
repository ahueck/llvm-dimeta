// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

typedef struct s1_t {
  char a;
} s1;

typedef struct s2_t {
  s1 a;
  s1 b;
} s2;

int main(void) {
  s2 c;
  return 0;
}

// CHECK: Line:            13
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT: Compound:
// CHECK-NEXT:   Name:            s2_t
// CHECK-NEXT:   Type:            struct
// CHECK-NEXT:   Extent:          2
// CHECK-NEXT:   Sizes:           [ 1, 1 ]
// CHECK-NEXT:   Offsets:         [ 0, 1 ]
// CHECK-NEXT:   Members:
// CHECK-NEXT:     - Name:            a
// CHECK-NEXT:       Builtin:         false
// CHECK-NEXT:       Type:
// CHECK-NEXT:         Compound:
// CHECK-NEXT:           Name:            s1_t
// CHECK-NEXT:           Type:            struct
// CHECK-NEXT:           Extent:          1
// CHECK-NEXT:           Sizes:           [ 1 ]
// CHECK-NEXT:           Offsets:         [ 0 ]
// CHECK-NEXT:           Members:
// CHECK-NEXT:             - Name:            a
// CHECK-NEXT:               Builtin:         true
// CHECK-NEXT:               Type:
// CHECK-NEXT:                 Fundamental:     { Name: char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT:         Typedef:         s1
// CHECK-NEXT:     - Name:            b
// CHECK-NEXT:       Builtin:         false
// CHECK-NEXT:       Type:
// CHECK-NEXT:         Compound:
// CHECK-NEXT:           Name:            s1_t
// CHECK-NEXT:           Type:            struct
// CHECK-NEXT:           Extent:          1
// CHECK-NEXT:           Sizes:           [ 1 ]
// CHECK-NEXT:           Offsets:         [ 0 ]
// CHECK-NEXT:           Members:
// CHECK-NEXT:             - Name:            a
// CHECK-NEXT:               Builtin:         true
// CHECK-NEXT:               Type:
// CHECK-NEXT:                 Fundamental:     { Name: char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT:         Typedef:         s1
// CHECK-NEXT: Typedef:         s2
