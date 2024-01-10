// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

struct VoidHolder {
  void*** a;
};

void* foo() {
  struct VoidHolder holder;
  holder.a = 0;
  return holder.a;
}

// CHECK: SourceLoc:
// CHECK-NEXT:   File:            {{.*}}stack_void_ptr.c{{'?}}
// CHECK-NEXT:   Function:        foo
// CHECK-NEXT:   Line:            8
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            VoidHolder
// CHECK-NEXT:     Type:            struct
// CHECK-NEXT:     Extent:          8
// CHECK-NEXT:     Sizes:           [ 8 ]
// CHECK-NEXT:     Offsets:         [ 0 ]
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            a
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: void, Extent: 8, Encoding: void_ptr }
// CHECK-NEXT:           Qualifiers:      [ ptr, ptr, ptr ]
