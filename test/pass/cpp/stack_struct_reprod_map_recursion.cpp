// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 -yaml | %filecheck %s

// XFAIL: *

// Extracted, simplified from std::map. Base_ptr causes endless recursion.

struct TreeNode {
  typedef TreeNode* Base_ptr;
  Base_ptr parent;
};

void foo() {
  const TreeNode impl;
}