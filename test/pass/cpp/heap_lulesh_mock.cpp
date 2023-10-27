// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

typedef float Real_t;

struct Domain {
  int numNode();
};

void foo(Domain& domain) {
  float* coords[3];
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK-NEXT: Pointer level: 1
  coords[0] = new Real_t[domain.numNode()];
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK-NEXT: Pointer level: 1
  coords[1] = new Real_t[domain.numNode()];
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
  // CHECK-NEXT: Pointer level: 1
  coords[2] = new Real_t[domain.numNode()];
}
