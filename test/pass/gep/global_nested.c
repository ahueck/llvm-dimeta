// RUN: %c-to-llvm %s | %opt -instcombine -S -o - |%apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)

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

char take_field() {
  //  foo chunky;
  chunky.bar.baz.quux.field5[17] = (char*)malloc(1);
  return 'a';
}