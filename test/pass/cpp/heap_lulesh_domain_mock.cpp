// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %cpp-to-llvm %s | %opt -O3 -S | %apply-verifier 2>&1 | %filecheck %s

class Domain {
  int ranks_, col_, row_;

 public:
  Domain(int ranks, int col, int row) : ranks_{ranks}, col_{col}, row_{row} {
  }

  int numNode() {
    return ranks_;
  }
};

void foo(Domain** domain, int numRanks, int col, int row) {
  // CHECK: Final Type: {{.*}} = distinct !DICompositeType(tag: DW_TAG_class_type, name: "Domain"
  // CHECK-NEXT: Pointer level: 1
  *domain = new Domain(numRanks, col, row);
}
