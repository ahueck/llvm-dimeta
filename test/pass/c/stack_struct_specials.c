// RUN: %c-to-llvm -std=c99 %s | %apply-verifier -yaml 2>&1 | %filecheck %s

#include <complex.h>
#include <stddef.h>
#include <stdint.h>

struct SpecialBuiltInTypes {
  _Bool boolean_var;  // Boolean type (C99)

  // Complex types (from <complex.h>, C99)
  float complex float_complex_var;              // Single-precision complex
  double complex double_complex_var;            // Double-precision complex
  long double complex long_double_complex_var;  // Extended-precision complex

  // float _Imaginary float_imaginary_var;              // Single-precision imaginary
  // double _Imaginary double_imaginary_var;            // Double-precision imaginary
  // long double _Imaginary long_double_imaginary_var;  // Extended-precision imaginary

  // _Decimal32 decimal32_var;    // 32-bit decimal floating-point
  // _Decimal64 decimal64_var;    // 64-bit decimal floating-point
  // _Decimal128 decimal128_var;  // 128-bit decimal floating-point
};

void foo() {
  struct SpecialBuiltInTypes types;
}

// CHECK: Members:
// CHECK-NEXT:   - Name:            boolean_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: {{bool|_Bool}}, Extent: 1, Encoding: bool }
// CHECK-NEXT:   - Name:            float_complex_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: complex, Extent: 8, Encoding: complex }
// CHECK-NEXT:   - Name:            double_complex_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: complex, Extent: 16, Encoding: complex }
// CHECK-NEXT:   - Name:            long_double_complex_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: complex, Extent: 32, Encoding: complex }
