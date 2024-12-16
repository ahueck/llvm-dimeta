// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// XFAIL: *

void f(int);

void foo() {
  void (*p1)(int) = f;
}

// !12 = !DILocalVariable(name: "p1", scope: !7, file: !8, line: 8, type: !13)
// !13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
// !14 = !DISubroutineType(types: !15)
// !15 = !{null, !16}
// !16 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
