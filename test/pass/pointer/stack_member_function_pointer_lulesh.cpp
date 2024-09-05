// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// XFAIL: *

class Domain {};

typedef double& (Domain::*Domain_member)(int);

void foo() {
  Domain_member fieldData[3];
}

// clang-format off
// !12 = !DILocalVariable(name: "fieldData", scope: !7, file: !8, line: 20, type: !13)

// !13 = !DICompositeType(tag: DW_TAG_array_type, baseType: !14, size: 384, elements: !23)
// !14 = !DIDerivedType(tag: DW_TAG_typedef, name: "Domain_member", file: !8, line: 17, baseType: !15)
// !15 = !DIDerivedType(tag: DW_TAG_ptr_to_member_type, baseType: !16, size: 128, extraData: !21)

// !16 = !DISubroutineType(types: !17)
// !17 = !{!18, !20, !22}
// !18 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !19, size: 64)
// !19 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
// !20 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !21, size: 64, flags: DIFlagArtificial | DIFlagObjectPointer)
// !21 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "Domain", file: !8, line: 3, size: 8, flags: DIFlagFwdDecl, identifier: "_ZTS6Domain")
// !22 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// !23 = !{!24}
// !24 = !DISubrange(count: 3)
