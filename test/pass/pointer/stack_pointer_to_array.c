// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

void foo() {
  int(*ap)[2];
}

// CHECK: Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT: Array:           [ 2 ]
// CHECK-NEXT: Qualifiers:      [ ptr, array ]

// !12 = !DILocalVariable(name: "ap", scope: !7, file: !8, line: 6, type: !13)
// !13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
// !14 = !DICompositeType(tag: DW_TAG_array_type, baseType: !15, size: 64, elements: !16)
// !15 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// !16 = !{!17}
// !17 = !DISubrange(count: 2)