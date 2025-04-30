// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

typedef struct s1_t {
  char a[3];       //  0
  struct s1_t* b;  //  8
} s1;

typedef struct s2_t {
  s1 a;            //  0
  s1* b;           //  16
  struct s2_t* c;  //  24
} s2;

typedef struct s3_t {
  s1 a[2];   //  0
  char b;    //  32
  s2* c[3];  //  40
} s3;

int main(void) {
  s3 c;
  return 0;
}

// CHECK:    Compound:
// CHECK-NEXT:      Name:            s3_t
// CHECK-NEXT:      Type:            struct
// CHECK-NEXT:      Extent:          64
// CHECK-NEXT:      Sizes:           [ 32, 1, 24 ]
// CHECK-NEXT:      Offsets:         [ 0, 32, 40 ]
// CHECK-NEXT:      Members:
// CHECK-NEXT:        - Name:            a
// CHECK-NEXT:          Builtin:         false
// CHECK-NEXT:          Type:
// CHECK-NEXT:            Compound:
// CHECK-NEXT:              Name:            s1_t
// CHECK-NEXT:              Type:            struct
// CHECK-NEXT:              Extent:          16
// CHECK-NEXT:              Sizes:           [ 3, 8 ]
// CHECK-NEXT:              Offsets:         [ 0, 8 ]
//                        ...
// CHECK:            Array:           [ 2 ]
// CHECK-NEXT:       Qualifiers:      [ array ]
// CHECK-NEXT:            Typedef:         s1
// CHECK-NEXT:        - Name:            b
// CHECK-NEXT:          Builtin:         true
// CHECK-NEXT:          Type:
// CHECK-NEXT:            Fundamental:     { Name: char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT:        - Name:            c
// CHECK-NEXT:          Builtin:         false
// CHECK-NEXT:          Type:
// CHECK-NEXT:            Compound:
// CHECK-NEXT:              Name:            s2_t
// CHECK-NEXT:              Type:            struct
// CHECK-NEXT:              Extent:          32
// CHECK-NEXT:              Sizes:           [ 16, 8, 8 ]
// CHECK-NEXT:              Offsets:         [ 0, 16, 24 ]
//                        ...
// CHECK:       Qualifiers:      [ array, ptr ]
// CHECK-NEXT:            Typedef:         s2
// CHECK-NEXT:    Typedef:         s3