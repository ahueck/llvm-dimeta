// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -instcombine -S -o - |%apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

// TODO handle nested constant geps without "instcombine"

#include "stdlib.h"

struct foo {
  //  int ss;
  struct {
    long field1;

    struct {
      long field2;
      long field3;

      //      union {
      struct most_inner {
        long field4;
        char* field5[32];
      } quux;

      long field6;
      long field7;
    } baz;

    long field8;
  } bar;
};

struct foo chunky;
struct foo* chunky2;

void take_field() {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
  chunky.bar.baz.quux.field5[17] = (char*)malloc(1);
}

void take_field_ptr() {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
  chunky2->bar.baz.quux.field5[0] = (char*)malloc(1);
}