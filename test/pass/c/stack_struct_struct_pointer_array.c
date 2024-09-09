// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

typedef struct s2_t {
  int x;
} s2;

typedef struct s3_t {
  s2* a[3];
} s3;

int main(void) {
  s3 s;
  return 0;
}

// CHECK:  Builtin:         false
// CHECK-NEXT:  Type:
// CHECK-NEXT:    Compound:
// CHECK-NEXT:      Name:            s3_t
// CHECK-NEXT:      Type:            struct
// CHECK-NEXT:      Extent:          24
// CHECK-NEXT:      Sizes:           [ 24 ]
// CHECK-NEXT:      Offsets:         [ 0 ]
// CHECK-NEXT:      Members:
// CHECK-NEXT:        - Name:            a
// CHECK-NEXT:          Builtin:         false
// CHECK-NEXT:          Type:
// CHECK-NEXT:            Compound:
// CHECK-NEXT:              Name:            s2_t
// CHECK-NEXT:              Type:            struct
// CHECK-NEXT:              Extent:          4
// CHECK-NEXT:              Sizes:           [ 4 ]
// CHECK-NEXT:              Offsets:         [ 0 ]
// CHECK-NEXT:              Members:
// CHECK-NEXT:                - Name:            x
// CHECK-NEXT:                  Builtin:         true
// CHECK-NEXT:                  Type:
// CHECK-NEXT:                    Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:            Array:           3
// CHECK-NEXT:            Qualifiers:      [ array, ptr ]
// CHECK-NEXT:            Typedef:         s2
// CHECK-NEXT:    Typedef:         s3