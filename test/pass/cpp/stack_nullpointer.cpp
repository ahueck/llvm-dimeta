// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <cstddef>

void foo() {
  std::nullptr_t null_ptr;
}

// CHECK: Final Type Stack: {{.*}} = !DIBasicType(tag: DW_TAG_unspecified_type, name: "decltype(nullptr)"

// CHECK:   Line:            6
// CHECK-NEXT: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: 'decltype(nullptr)', Extent: 8, Encoding: nullptr }
// CHECK-NEXT:   Typedef:         nullptr_t
