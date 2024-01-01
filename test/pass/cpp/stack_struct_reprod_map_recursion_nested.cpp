// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 -dump | %filecheck %s

// CHECK: Typedef: {{ *}}Base_ptr
// CHECK-NEXT: Recurring: {{ *}}true

struct TreeNode {
  struct NestedNode {
    typedef TreeNode* Base_ptr;
    Base_ptr parent;
  };
  NestedNode n;
};

void foo() {
  TreeNode impl;
}