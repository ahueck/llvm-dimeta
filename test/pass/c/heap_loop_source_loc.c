// RUN: %c-to-llvm %s | %apply-verifier |& %filecheck %s
// RUN: %c-to-llvm %s | %opt -O2 | %apply-verifier |& %filecheck %s

#include <stdlib.h>

void foo(int** entries, int n) {
  for (int i = 0; i < 4; ++i) {
    entries[i] = malloc(sizeof(int) * n);
  }
}

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "int",
// CHECK-NEXT: Pointer level: 1 (T*)

// CHECK: Location: "{{.*}}heap_loop_source_loc.c":"foo":8