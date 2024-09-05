// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

class Domain {
 public:
  int m;
};

void foo() {
  int Domain::*pointer_to = &Domain::m;
}

// CHECK: Builtin:         true
// CHECK-NEXT: Type:
// CHECK-NEXT:   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   Qualifiers:      [ ptr_to_mem ]

// !12 = !DILocalVariable(name: "pointer_to", scope: !7, file: !8, line: 11, type: !13)
// !13 = !DIDerivedType(tag: DW_TAG_ptr_to_member_type, baseType: !14, size: 64, extraData: !15)
// !14 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// !15 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "Domain", file: !8, line: 5, size: 32, flags:
// DIFlagTypePassByValue, elements: !16, identifier: "_ZTS6Domain")