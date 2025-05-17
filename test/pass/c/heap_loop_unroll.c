// RUN: %c-to-llvm %s | %opt -O3 | %apply-verifier |& %filecheck %s

#include <stdlib.h>

void handle(int* [4]);
void foo(int n) {
  int* entries[4];
#pragma unroll
  for (int i = 0; i < 4; ++i) {
    entries[i] = (int*)malloc(sizeof(int));
  }
  handle(entries);
}

// CHECK-COUNT-4: Location: "{{.*}}heap_loop_unroll.c":"foo":10
