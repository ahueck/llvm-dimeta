// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type Global: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// CHECK: Pointer level: 0
// CHECK: Location: "{{.*}}":"":7

int a;

int foo() {
  return a + 1;
}
