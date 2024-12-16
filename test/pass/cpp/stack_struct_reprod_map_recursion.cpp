// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 -yaml | %filecheck %s

struct TreeNode {
  typedef TreeNode* Base_ptr;
  Base_ptr parent;
};

void foo() {
  const TreeNode impl{};
}

// CHECK: Name:            TreeNode
// CHECK-NEXT: Identifier:      _ZTS8TreeNode
// CHECK-NEXT: Type:            struct
// CHECK-NEXT: Extent:          8
// CHECK-NEXT: Sizes:           [ 8 ]
// CHECK-NEXT: Offsets:         [ 0 ]
// CHECK-NEXT: Members:
// CHECK-NEXT:       - Name:            parent
// CHECK-NEXT:         Builtin:         false
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            TreeNode
// CHECK-NEXT:             Identifier:      _ZTS8TreeNode
// CHECK-NEXT:             Type:            struct
// CHECK-NEXT:             Extent:          8
// CHECK-NEXT:           Qualifiers:      [ ptr ]
// CHECK-NEXT:           Typedef:         Base_ptr
// CHECK-NEXT:           Recurring:       true