// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 -yaml | %filecheck %s

// XFAIL: *

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