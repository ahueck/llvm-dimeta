// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

typedef struct {
  void (*freetex)(void*);
} texture_methods;

void foo() {
  texture_methods methods;
}

// CHECK: Name:            ''
// CHECK-NEXT: Type:            struct
// CHECK-NEXT: Extent:          8
// CHECK-NEXT: Sizes:           [ 8 ]
// CHECK-NEXT: Offsets:         [ 0 ]
// CHECK-NEXT: Members:
// CHECK-NEXT:   - Name:            freetex
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: '', Extent: 8, Encoding: function_ptr }
// CHECK-NEXT:       Qualifiers:      [ ptr ]
// CHECK-NEXT: Typedef:         texture_methods
