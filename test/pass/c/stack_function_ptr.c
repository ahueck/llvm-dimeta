// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

void foo() {
  void (*freetex)(void*);
}

// CHECK:   Fundamental:     { Name: '', Extent: 8, Encoding: function_ptr }
// CHECK-NEXT:   Qualifiers:      [ ptr ]
