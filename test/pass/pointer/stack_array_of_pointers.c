// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// XFAIL: *

void foo() {
  void (*a[10])(int);
}

// !12 = !DILocalVariable(name: "a", scope: !7, file: !8, line: 6, type: !13)
// !13 = !DICompositeType(tag: DW_TAG_array_type, baseType: !14, size: 640, elements: !18)
// !14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64)
// !15 = !DISubroutineType(types: !16)
// !16 = !{null, !17}
// !17 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// !18 = !{!19}
// !19 = !DISubrange(count: 10)