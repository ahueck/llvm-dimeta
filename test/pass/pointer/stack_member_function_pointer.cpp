// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// XFAIL: *

class Domain {};

using Domain_member = void (Domain::*)();

void foo() {
  Domain_member fieldData;
}

// !12 = !DILocalVariable(name: "fieldData", scope: !7, file: !8, line: 10, type: !13)
// !13 = !DIDerivedType(tag: DW_TAG_typedef, name: "Domain_member", file: !8, line: 7, baseType: !14)
// !14 = !DIDerivedType(tag: DW_TAG_ptr_to_member_type, baseType: !15, size: 128, extraData: !18)
// !15 = !DISubroutineType(types: !16)
// !16 = !{null, !17}
// !17 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !18, size: 64, flags: DIFlagArtificial |
// DIFlagObjectPointer) !18 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "Domain", file: !8, line: 5, size:
// 8, flags: DIFlagFwdDecl, identifier: "_ZTS6Domain")