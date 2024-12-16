// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

void foo() {
  // An array [2] of pointers to arrays of 3 integers
  int(*array_of_pointers[2])[3];
}

// CHECK:   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   Array:           [ 2, 3 ]
// CHECK-NEXT:   Qualifiers:      [ array, ptr, array ]

// !13 = !DILocalVariable(name: "array_of_pointers", scope: !8, file: !9, line: 6, type: !14)
// !14 = !DICompositeType(tag: DW_TAG_array_type, baseType: !15, size: 128, elements: !20)
// !15 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !16, size: 64)
// !16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 96, elements: !18)
// !17 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// !18 = !{!19}
// !19 = !DISubrange(count: 3)
// !20 = !{!21}
// !21 = !DISubrange(count: 2)