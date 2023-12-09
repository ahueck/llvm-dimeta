// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -instcombine -S -o - |%apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O3 -S | %apply-verifier 2>&1 | %filecheck %s

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

// CHECK: Final Type Global: {{.*}} = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "foo"
// CHECK: Pointer level: 0
struct foo chunky;
// CHECK: Extracted Type Global: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREFG:![0-9]+]]
// CHECK: Final Type Global: [[DIREFG]] = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "foo"
// CHECK: Pointer level: 1
struct foo* chunky2;

void take_field() {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
  // CHECK-NEXT: Pointer level: 1
  chunky.bar.baz.quux.field5[17] = (char*)malloc(1);
}

void take_field_ptr() {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF2:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF2]] = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
  // CHECK-NEXT: Pointer level: 1
  chunky2->bar.baz.quux.field5[0] = (char*)malloc(1);
}