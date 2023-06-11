// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: T: [store ptr %2, ptr %1{{.*}} --> %1 = alloca ptr{{.*}}]
// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)

#include <stdlib.h>

void foo() {
  int* p = malloc(sizeof(int) * 4);
}
