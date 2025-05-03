// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

typedef struct {
  double (*shader)(void*);
  float (*phongfunc)(const float* incident, const int* shadevars, float specpower);
  int transmode;
} texture_methods;

void foo() {
  texture_methods methods;
}

// CHECK: Name:            ''
// CHECK-NEXT: Type:            struct
// CHECK-NEXT: Extent:          24
// CHECK-NEXT: Sizes:           [ 8, 8, 4 ]
// CHECK-NEXT: Offsets:         [ 0, 8, 16 ]
// CHECK-NEXT: Members:
// CHECK-NEXT:   - Name:            shader
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: '', Extent: 8, Encoding: function_ptr }
// CHECK-NEXT:       Qualifiers:      [ ptr ]
// CHECK-NEXT:   - Name:            phongfunc
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: '', Extent: 8, Encoding: function_ptr }
// CHECK-NEXT:       Qualifiers:      [ ptr ]
// CHECK-NEXT:   - Name:            transmode
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT: Typedef:         texture_methods
