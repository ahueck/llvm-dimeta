; RUN: %apply-verifier %s |& %filecheck %s
; REQUIRES: llvm-18 || llvm-19
; CHECK-NOT: Assertion


; CHECK:   Line:            32
; CHECK-NEXT: Builtin:         true
; CHECK-NEXT: Type:
; CHECK-NEXT:   Fundamental:     { Name: double, Extent: 8, Encoding: float }
; CHECK-NEXT:   Qualifiers:      [ ptr ]

source_filename = "gep/param_first_nested.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress uwtable
define dso_local void @_Z10take_fieldP3foo(ptr nocapture noundef writeonly %chunky2) local_unnamed_addr #0 !dbg !12 {
entry:
    #dbg_value(ptr %chunky2, !40, !DIExpression(), !41)
  %baz = alloca ptr
    #dbg_value(ptr %baz, !40, !DIExpression(), !41)
  %call = tail call ptr @malloc(i32 noundef 8), !dbg !42
  ; %baz = getelementptr inbounds i8, ptr %chunky2, i64 8, !dbg !43
  store ptr %call, ptr %baz, align 8, !dbg !44, !tbaa !45
  ret void, !dbg !54
}

declare !dbg !55 ptr @malloc(i32 noundef) local_unnamed_addr #1

attributes #0 = { mustprogress uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!5, !6, !7, !8, !9, !10}
!llvm.ident = !{!11}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !1, producer: "clang version 19.1.6", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "gep/param_first_nested.cpp", directory: "dimeta/build_19", checksumkind: CSK_MD5, checksum: "776f4abb4b03faded1904660ab998251")
!2 = !{!3}
!3 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!4 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!5 = !{i32 7, !"Dwarf Version", i32 5}
!6 = !{i32 2, !"Debug Info Version", i32 3}
!7 = !{i32 1, !"wchar_size", i32 4}
!8 = !{i32 8, !"PIC Level", i32 2}
!9 = !{i32 7, !"PIE Level", i32 2}
!10 = !{i32 7, !"uwtable", i32 2}
!11 = !{!"clang version 19.1.6"}
!12 = distinct !DISubprogram(name: "take_field", linkageName: "_Z10take_fieldP3foo", scope: !13, file: !13, line: 30, type: !14, scopeLine: 30, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !39)
!13 = !DIFile(filename: "test/pass/gep/param_first_nested.cpp", directory: "dimeta", checksumkind: CSK_MD5, checksum: "776f4abb4b03faded1904660ab998251")
!14 = !DISubroutineType(types: !15)
!15 = !{null, !16}
!16 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
!17 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "foo", file: !13, line: 11, size: 192, flags: DIFlagTypePassByValue, elements: !18, identifier: "_ZTS3foo")
!18 = !{!19}
!19 = !DIDerivedType(tag: DW_TAG_member, name: "bar", scope: !17, file: !13, line: 27, baseType: !20, size: 192)
!20 = distinct !DICompositeType(tag: DW_TAG_structure_type, scope: !17, file: !13, line: 12, size: 192, flags: DIFlagTypePassByValue, elements: !21, identifier: "_ZTSN3fooUt_E")
!21 = !{!22}
!22 = !DIDerivedType(tag: DW_TAG_member, name: "foobar", scope: !20, file: !13, line: 26, baseType: !23, size: 192)
!23 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "xx", scope: !20, file: !13, line: 13, size: 192, flags: DIFlagTypePassByValue, elements: !24, identifier: "_ZTSN3fooUt_2xxE")
!24 = !{!25, !27}
!25 = !DIDerivedType(tag: DW_TAG_member, name: "padding", scope: !23, file: !13, line: 14, baseType: !26, size: 32)
!26 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!27 = !DIDerivedType(tag: DW_TAG_member, name: "baz", scope: !23, file: !13, line: 25, baseType: !28, size: 128, offset: 64)
!28 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "bb", scope: !23, file: !13, line: 15, size: 128, flags: DIFlagTypePassByValue, elements: !29, identifier: "_ZTSN3fooUt_2xx2bbE")
!29 = !{!30, !31, !34, !38}
!30 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !28, file: !13, line: 22, baseType: !3, size: 64)
!31 = !DIDerivedType(tag: DW_TAG_member, name: "z", scope: !28, file: !13, line: 23, baseType: !32, size: 64, offset: 64)
!32 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !33, size: 64)
!33 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!34 = !DISubprogram(name: "get_x", linkageName: "_ZN3fooUt_2xx2bb5get_xEv", scope: !28, file: !13, line: 16, type: !35, scopeLine: 16, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized)
!35 = !DISubroutineType(types: !36)
!36 = !{!3, !37}
!37 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !28, size: 64, flags: DIFlagArtificial | DIFlagObjectPointer)
!38 = !DISubprogram(name: "get_z", linkageName: "_ZN3fooUt_2xx2bb5get_zEv", scope: !28, file: !13, line: 19, type: !35, scopeLine: 19, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized)
!39 = !{!40}
!40 = !DILocalVariable(name: "chunky2", arg: 1, scope: !12, file: !13, line: 30, type: !16)
!41 = !DILocation(line: 0, scope: !12)
!42 = !DILocation(line: 32, column: 40, scope: !12)
!43 = !DILocation(line: 32, column: 23, scope: !12)
!44 = !DILocation(line: 32, column: 29, scope: !12)
!45 = !{!46, !53, i64 8}
!46 = !{!"_ZTS3foo", !47, i64 0}
!47 = !{!"_ZTSN3fooUt_E", !48, i64 0}
!48 = !{!"_ZTSN3fooUt_2xxE", !49, i64 0, !52, i64 8}
!49 = !{!"int", !50, i64 0}
!50 = !{!"omnipotent char", !51, i64 0}
!51 = !{!"Simple C++ TBAA"}
!52 = !{!"_ZTSN3fooUt_2xx2bbE", !53, i64 0, !53, i64 8}
!53 = !{!"any pointer", !50, i64 0}
!54 = !DILocation(line: 33, column: 1, scope: !12)
!55 = !DISubprogram(name: "malloc", scope: !13, file: !13, line: 9, type: !56, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized)
!56 = !DISubroutineType(types: !57)
!57 = !{!58, !59}
!58 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!59 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)