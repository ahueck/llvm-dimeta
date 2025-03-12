; RUN: %apply-verifier %s |& %filecheck %s
; REQUIRES: llvm-19
; CHECK-NOT: Assertion

; CHECK:   Line:            32
; CHECK-NEXT: Builtin:         true
; CHECK-NEXT: Type:
; CHECK-NEXT:   Fundamental:     { Name: double, Extent: 8, Encoding: float }
; CHECK-NEXT:   Qualifiers:      [ ptr ]

target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @_Z10take_fieldP3foo() {
entry:
  %baz = alloca ptr, i32 0, align 8
    #dbg_value(ptr %baz, !4, !DIExpression(), !23)
  %call = call ptr @malloc(), !dbg !24
  store ptr %call, ptr %baz, align 8, !tbaa !25
  ret void
}

declare ptr @malloc()

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !1, producer: "clang version 19.1.6", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "gep/param_first_nested.cpp", directory: "dimeta/build_19", checksumkind: CSK_MD5, checksum: "776f4abb4b03faded1904660ab998251")
!2 = !{}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !DILocalVariable(name: "chunky2", arg: 1, scope: !5, file: !6, line: 30, type: !8)
!5 = distinct !DISubprogram(name: "take_field", linkageName: "_Z10take_fieldP3foo", scope: !6, file: !6, line: 30, type: !7, scopeLine: 30, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !2)
!6 = !DIFile(filename: "test/pass/gep/param_first_nested.cpp", directory: "dimeta", checksumkind: CSK_MD5, checksum: "776f4abb4b03faded1904660ab998251")
!7 = distinct !DISubroutineType(types: !2)
!8 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !9, size: 64)
!9 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "foo", file: !6, line: 11, size: 192, flags: DIFlagTypePassByValue, elements: !10, identifier: "_ZTS3foo")
!10 = !{!11}
!11 = !DIDerivedType(tag: DW_TAG_member, name: "bar", scope: !9, file: !6, line: 27, baseType: !12, size: 192)
!12 = distinct !DICompositeType(tag: DW_TAG_structure_type, scope: !9, file: !6, line: 12, size: 192, flags: DIFlagTypePassByValue, elements: !13, identifier: "_ZTSN3fooUt_E")
!13 = !{!14}
!14 = !DIDerivedType(tag: DW_TAG_member, name: "foobar", scope: !12, file: !6, line: 26, baseType: !15, size: 192)
!15 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "xx", scope: !12, file: !6, line: 13, size: 192, flags: DIFlagTypePassByValue, elements: !16, identifier: "_ZTSN3fooUt_2xxE")
!16 = !{!17}
!17 = !DIDerivedType(tag: DW_TAG_member, name: "baz", scope: !15, file: !6, line: 25, baseType: !18, size: 128, offset: 64)
!18 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "bb", scope: !15, file: !6, line: 15, size: 128, flags: DIFlagTypePassByValue, elements: !19, identifier: "_ZTSN3fooUt_2xx2bbE")
!19 = !{!20}
!20 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !18, file: !6, line: 22, baseType: !21, size: 64)
!21 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !22, size: 64)
!22 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!23 = !DILocation(line: 0, scope: !5)
!24 = !DILocation(line: 32, column: 40, scope: !5)
!25 = !{!26, !33, i64 8}
!26 = !{!"_ZTS3foo", !27, i64 0}
!27 = !{!"_ZTSN3fooUt_E", !28, i64 0}
!28 = !{!"_ZTSN3fooUt_2xxE", !29, i64 0, !32, i64 8}
!29 = !{!"int", !30, i64 0}
!30 = !{!"omnipotent char", !31, i64 0}
!31 = !{!"Simple C++ TBAA"}
!32 = !{!"_ZTSN3fooUt_2xx2bbE", !33, i64 0, !33, i64 8}
!33 = !{!"any pointer", !30, i64 0}
