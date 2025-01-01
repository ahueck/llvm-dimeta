// RUN: %c-to-llvm -std=c2x %s | %apply-verifier -yaml 2>&1 | %filecheck %s

// REQUIRES: !legacyllvm

#include <stddef.h>
#include <stdint.h>

struct SpecialBuiltInTypes {
#if __clang_major__ >= 14
  _BitInt(8) bitint8_var;
  _BitInt(16) bitint16_var;
  _BitInt(32) bitint32_var;
#endif
};

void foo() {
  struct SpecialBuiltInTypes types;
}

// CHECK: Members:
// CHECK-NEXT:   - Name:            bitint8_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: _BitInt, Extent: 1, Encoding: signed_int }
// CHECK-NEXT:   - Name:            bitint16_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: _BitInt, Extent: 2, Encoding: signed_int }
// CHECK-NEXT:   - Name:            bitint32_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: _BitInt, Extent: 4, Encoding: signed_int }
