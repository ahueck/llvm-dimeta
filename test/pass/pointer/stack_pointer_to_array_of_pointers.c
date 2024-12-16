// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

void foo() {
  // int* arr[3];
  int*(*ptr_to_arr)[3]; /* = arr;*/
}

// CHECK: Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT: Array:           [ 3 ]
// CHECK-NEXT: Qualifiers:      [ ptr, array, ptr ]

// !13 = !DILocalVariable(name: "ptr_to_arr", scope: !8, file: !9, line: 4, type: !14)
// !14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64)
// !15 = !DICompositeType(tag: DW_TAG_array_type, baseType: !16, size: 192, elements: !18)
// !16 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
// !17 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// !18 = !{!19}
// !19 = !DISubrange(count: 3)