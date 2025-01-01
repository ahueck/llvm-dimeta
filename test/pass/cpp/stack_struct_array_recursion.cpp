// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 -yaml | %filecheck %s

struct TreeNode {
  typedef TreeNode* Base_ptr;
  Base_ptr parent[3];
};

void foo() {
  const TreeNode impl{};
}

// CHECK: - Name:            parent
// CHECK-NEXT:         Builtin:         false
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            TreeNode
// CHECK-NEXT:             Identifier:      _ZTS8TreeNode
// CHECK-NEXT:             Type:            struct
// CHECK-NEXT:             Extent:          24
// CHECK-NEXT:           Array:           [ 3 ]
// CHECK-NEXT:           Qualifiers:      [ array, ptr ]
// CHECK-NEXT:           Typedef:         Base_ptr
// CHECK-NEXT:           Recurring:       true