// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %cpp-to-llvm %s | %opt -O3 -S | %apply-verifier 2>&1 | %filecheck %s

char** multimeshObjs;
char** multimatObjs;
char*** multivarObjs;

void foo(int numRanks) {
  char vars[][10] = {"p", "e", "v", "q", "speed", "xd", "yd", "zd"};
  int numvars     = sizeof(vars) / sizeof(vars[0]);
  //   CHECK: Final Type: {{.*}} = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
  //   CHECK-NEXT: Pointer level: 2
  multimeshObjs = new char*[numRanks];
  //   CHECK: Final Type: {{.*}} = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
  //   CHECK-NEXT: Pointer level: 2
  multimatObjs = new char*[numRanks];
  //   CHECK: Final Type: {{.*}} = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
  //   CHECK-NEXT: Pointer level: 3
  multivarObjs = new char**[numvars * 2];

  for (int i = 0; i < numRanks; ++i) {
    for (int v = 0; v < numvars; ++v) {
      // CHECK: Final Type: {{.*}} = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
      // CHECK-NEXT: Pointer level: 1
      multivarObjs[v][i] = new char[64];
    }
  }
}
