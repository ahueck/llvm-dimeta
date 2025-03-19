; RUN: %apply-verifier %s |& %filecheck %s
; REQUIRES: llvm-18 || llvm-19

; CHECK: Type for heap-like:   %call = call ptr @realloc(), !dbg !44
; CHECK: Extracted Type: !26 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !27, size: 64)
; CHECK: Final Type: !31 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
; CHECK: Pointer level: 1

%"struct.codi::TapeBaseModule" = type <{ %"class.codi::ReverseTapeInterface", ptr, i32, i8, [3 x i8] }>
%"class.codi::ReverseTapeInterface" = type { %"class.codi::TapeInterface" }
%"class.codi::TapeInterface" = type { ptr }

define void @_ZN4codi14TapeBaseModuleINS_15JacobiTapeTypesINS_16ReverseTapeTypesIddNS_18LinearIndexHandlerIiEEEENS_11ChunkVectorEEENS_10JacobiTapeIS7_EEE14resizeAdjointsERKi() {
entry:
  %this.addr = alloca ptr, align 8
    ; #dbg_declare(ptr %this.addr, !3, !DIExpression(), !43)
  call void @llvm.dbg.declare(metadata ptr %this.addr, metadata !3, metadata !DIExpression()), !dbg !43
  %this1 = load ptr, ptr %this.addr, align 8
  %call = call ptr @realloc(), !dbg !44
  %adjoints6 = getelementptr inbounds %"struct.codi::TapeBaseModule", ptr %this1, i32 0, i32 1, !dbg !44
  store ptr %call, ptr %adjoints6, align 8, !dbg !44, !tbaa !14066
  ret void
}

  ; LLVM 14:
  ; %call = call i8* @realloc(i8* noundef %13, i64 noundef %mul) #17, !dbg !61134
  ; %15 = bitcast i8* %call to double*, !dbg !61135
  ; %adjoints6 = getelementptr inbounds %"struct.codi::TapeBaseModule", %"struct.codi::TapeBaseModule"* %this1, i32 0, i32 1, !dbg !61136
  ; store double* %15, double** %adjoints6, align 8, !dbg !61137, !tbaa !58144

declare ptr @realloc()

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !1, producer: "clang version 19.1.6", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "lulesh.cc", directory: "lulesh", checksumkind: CSK_MD5, checksum: "8d13b4cf5cd37f06475f0790e5054685")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !DILocalVariable(name: "this", arg: 1, scope: !4, type: !42, flags: DIFlagArtificial | DIFlagObjectPointer)
!4 = distinct !DISubprogram(name: "resizeAdjoints", linkageName: "_ZN4codi14TapeBaseModuleINS_15JacobiTapeTypesINS_16ReverseTapeTypesIddNS_18LinearIndexHandlerIiEEEENS_11ChunkVectorEEENS_10JacobiTapeIS7_EEE14resizeAdjointsERKi", scope: !6, file: !5, line: 177, type: !40, scopeLine: 177, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, declaration: !41, retainedNodes: !21)
!5 = !DIFile(filename: "codi/tapes/modules/tapeBaseModule.hpp", directory: "lulesh", checksumkind: CSK_MD5, checksum: "91343b7c0470eb61653deab98d32b277")
!6 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "TapeBaseModule<codi::JacobiTapeTypes<codi::ReverseTapeTypes<double, double, codi::LinearIndexHandler<int> >, codi::ChunkVector>, codi::JacobiTape<codi::JacobiTapeTypes<codi::ReverseTapeTypes<double, double, codi::LinearIndexHandler<int> >, codi::ChunkVector> > >", scope: !7, file: !5, line: 63, size: 192, flags: DIFlagTypePassByReference | DIFlagNonTrivial, elements: !8, vtableHolder: !14, templateParams: !21, identifier: "_ZTSN4codi14TapeBaseModuleINS_15JacobiTapeTypesINS_16ReverseTapeTypesIddNS_18LinearIndexHandlerIiEEEENS_11ChunkVectorEEENS_10JacobiTapeIS7_EEEE")
!7 = !DINamespace(name: "codi", scope: null)
!8 = !{!9, !22, !25, !32, !39}
!9 = !DIDerivedType(tag: DW_TAG_inheritance, scope: !6, baseType: !10, offset: 288, flags: DIFlagVirtual, extraData: i32 0)
!10 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "ReverseTapeInterface<double, int, double, codi::JacobiTape<codi::JacobiTapeTypes<codi::ReverseTapeTypes<double, double, codi::LinearIndexHandler<int> >, codi::ChunkVector> >, codi::ChunkVector<codi::Chunk2<codi::ExternalFunction, codi::ChunkVector<codi::Chunk2<double, int>, codi::ChunkVector<codi::Chunk1<unsigned char>, codi::LinearIndexHandler<int> > >::Position>, codi::ChunkVector<codi::Chunk2<double, int>, codi::ChunkVector<codi::Chunk1<unsigned char>, codi::LinearIndexHandler<int> > > >::Position>", scope: !7, file: !11, line: 64, size: 64, flags: DIFlagTypePassByReference | DIFlagNonTrivial, elements: !12, vtableHolder: !14, templateParams: !21, identifier: "_ZTSN4codi20ReverseTapeInterfaceIdidNS_10JacobiTapeINS_15JacobiTapeTypesINS_16ReverseTapeTypesIddNS_18LinearIndexHandlerIiEEEENS_11ChunkVectorEEEEENS7_INS_6Chunk2INS_16ExternalFunctionENS7_INSA_IdiEENS7_INS_6Chunk1IhEES5_EEE8PositionEEESG_E8PositionEEE")
!11 = !DIFile(filename: "codi/tapes/modules/../reverseTapeInterface.hpp", directory: "lulesh", checksumkind: CSK_MD5, checksum: "9d519cad455b51eae3adb72019b270a8")
!12 = !{!13}
!13 = !DIDerivedType(tag: DW_TAG_inheritance, scope: !10, baseType: !14, flags: DIFlagPublic, extraData: i32 0)
!14 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "TapeInterface<double, int, double>", scope: !7, file: !15, line: 57, size: 64, flags: DIFlagTypePassByReference | DIFlagNonTrivial, elements: !16, vtableHolder: !14, templateParams: !21, identifier: "_ZTSN4codi13TapeInterfaceIdidEE")
!15 = !DIFile(filename: "codi/tapes/tapeInterface.hpp", directory: "lulesh", checksumkind: CSK_MD5, checksum: "7987cce282380d931a7d112b198b227e")
!16 = !{!17}
!17 = !DIDerivedType(tag: DW_TAG_member, name: "_vptr$TapeInterface", scope: !15, file: !15, baseType: !18, size: 64, flags: DIFlagArtificial)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!19 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "__vtbl_ptr_type", baseType: !20, size: 64)
!20 = distinct !DISubroutineType(types: !21)
!21 = !{}
!22 = !DIDerivedType(tag: DW_TAG_variable, name: "LinearIndexHandler", scope: !6, file: !5, line: 103, baseType: !23, flags: DIFlagStaticMember, extraData: i1 true)
!23 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !24)
!24 = !DIBasicType(name: "bool", size: 8, encoding: DW_ATE_boolean)
!25 = !DIDerivedType(tag: DW_TAG_member, name: "adjoints", scope: !6, file: !5, line: 113, baseType: !26, size: 64, offset: 64, flags: DIFlagProtected)
!26 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !27, size: 64)
!27 = !DIDerivedType(tag: DW_TAG_typedef, name: "GradientValue", scope: !6, file: !5, line: 70, baseType: !28, flags: DIFlagPrivate)
!28 = !DIDerivedType(tag: DW_TAG_typedef, name: "GradientValue", scope: !30, file: !29, line: 95, baseType: !31)
!29 = !DIFile(filename: "codi/tapeTypes.hpp", directory: "lulesh", checksumkind: CSK_MD5, checksum: "5a7f1a66b1cc10a83efb529e1a2836f3")
!30 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "ReverseTapeTypes<double, double, codi::LinearIndexHandler<int> >", scope: !7, file: !29, line: 92, size: 8, flags: DIFlagTypePassByValue, elements: !21, templateParams: !21, identifier: "_ZTSN4codi16ReverseTapeTypesIddNS_18LinearIndexHandlerIiEEEE")
!31 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!32 = !DIDerivedType(tag: DW_TAG_member, name: "adjointsSize", scope: !6, file: !5, line: 116, baseType: !33, size: 32, offset: 128, flags: DIFlagProtected)
!33 = !DIDerivedType(tag: DW_TAG_typedef, name: "Index", scope: !6, file: !5, line: 70, baseType: !34, flags: DIFlagPrivate)
!34 = !DIDerivedType(tag: DW_TAG_typedef, name: "Index", scope: !30, file: !29, line: 97, baseType: !35)
!35 = !DIDerivedType(tag: DW_TAG_typedef, name: "Index", scope: !37, file: !36, line: 62, baseType: !38, flags: DIFlagPublic)
!36 = !DIFile(filename: "codi/tapes/indices/linearIndexHandler.hpp", directory: "lulesh", checksumkind: CSK_MD5, checksum: "f6abfadf8c5750cb28567dd3f84fb959")
!37 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "LinearIndexHandler<int>", scope: !7, file: !36, line: 56, size: 64, flags: DIFlagTypePassByValue | DIFlagNonTrivial, elements: !21, templateParams: !21, identifier: "_ZTSN4codi18LinearIndexHandlerIiEE")
!38 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!39 = !DIDerivedType(tag: DW_TAG_member, name: "active", scope: !6, file: !5, line: 121, baseType: !24, size: 8, offset: 160, flags: DIFlagProtected)
!40 = distinct !DISubroutineType(types: !21)
!41 = !DISubprogram(name: "resizeAdjoints", linkageName: "_ZN4codi14TapeBaseModuleINS_15JacobiTapeTypesINS_16ReverseTapeTypesIddNS_18LinearIndexHandlerIiEEEENS_11ChunkVectorEEENS_10JacobiTapeIS7_EEE14resizeAdjointsERKi", scope: !6, file: !5, line: 177, type: !40, scopeLine: 177, flags: DIFlagProtected | DIFlagPrototyped, spFlags: DISPFlagOptimized)
!42 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!43 = !DILocation(line: 1, scope: !4)
!44 = !DILocation(line: 185, column: 36, scope: !4)
!14045 = !{!14046, !14046, i64 0}
!14046 = !{!"int", !14047, i64 0}
!14047 = !{!"omnipotent char", !14048, i64 0}
!14048 = !{!"Simple C++ TBAA"}
!14050 = !{!14051, !14051, i64 0}
!14051 = !{!"any pointer", !14047, i64 0}
!14054 = !{!14055, !14046, i64 16}
!14055 = !{!"_ZTSN4codi14TapeBaseModuleINS_15JacobiTapeTypesINS_16ReverseTapeTypesIddNS_18LinearIndexHandlerIiEEEENS_11ChunkVectorEEENS_10JacobiTapeIS7_EEEE", !14051, i64 8, !14046, i64 16, !14056, i64 20}
!14056 = !{!"bool", !14047, i64 0}
!14066 = !{!14055, !14051, i64 8}
