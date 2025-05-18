; RUN: %apply-verifier %s |& %filecheck %s
; REQUIRES: llvm-18 || llvm-19

; CHECK:   Function:        'cudaMalloc<float>'
; CHECK-NEXT:   Line:            77
; CHECK-NEXT: Builtin:         true
; CHECK-NEXT: Type:
; CHECK-NEXT:   Fundamental:     { Name: float, Extent: 4, Encoding: float }
; CHECK-NEXT:   Qualifiers:      [ ptr, ptr ]

source_filename = "01_test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: inlinehint mustprogress sanitize_thread uwtable
define hidden noundef i32 @_ZL10cudaMallocIfE9cudaErrorPPT_m(ptr noundef %0, i64 noundef %1) #0 !dbg !12 {
  %3 = alloca ptr, align 8, !DIAssignID !15
  call void @llvm.dbg.assign(metadata ptr %3, metadata !16, metadata !DIExpression(), metadata !15, metadata ptr %3, metadata !DIExpression()), !dbg !20
  %4 = alloca i64, align 8, !DIAssignID !21
  call void @llvm.dbg.assign(metadata i1 undef, metadata !22, metadata !DIExpression(), metadata !21, metadata ptr %4, metadata !DIExpression()), !dbg !20
  store ptr %0, ptr %3, align 8, !tbaa !26, !DIAssignID !30
  call void @llvm.dbg.assign(metadata ptr %0, metadata !16, metadata !DIExpression(), metadata !30, metadata ptr %3, metadata !DIExpression()), !dbg !20
  store i64 %1, ptr %4, align 8, !tbaa !31, !DIAssignID !33
  call void @llvm.dbg.assign(metadata i64 %1, metadata !22, metadata !DIExpression(), metadata !33, metadata ptr %4, metadata !DIExpression()), !dbg !20
  %5 = load ptr, ptr %3, align 8, !dbg !34, !tbaa !26
  %6 = load i64, ptr %4, align 8, !dbg !35, !tbaa !31
  %7 = call i32 @cudaMalloc(ptr noundef %5, i64 noundef %6), !dbg !36
  ret i32 %7, !dbg !37
}

declare !dbg !38 i32 @cudaMalloc(ptr noundef, i64 noundef) #1

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.assign(metadata, metadata, metadata, metadata, metadata, metadata) #2

; uselistorder directives
uselistorder ptr @llvm.dbg.assign, { 3, 2, 1, 0 }


!llvm.module.flags = !{!0, !1, !2, !3, !4, !5, !6, !7}
!llvm.dbg.cu = !{!8}
!llvm.ident = !{!11}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 11, i32 8]}
!1 = !{i32 7, !"Dwarf Version", i32 5}
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{i32 8, !"PIC Level", i32 2}
!5 = !{i32 7, !"PIE Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 2}
!7 = !{i32 7, !"debug-info-assignment-tracking", i1 true}
!8 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !9, producer: "clang version 18.1.8 (https://github.com/llvm/llvm-project.git 3b5b5c1ec4a3095ab096dd780e84d7ab81f3d7ff)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !10, retainedTypes: !10, imports: !10, splitDebugInlining: false, nameTableKind: None)
!9 = !DIFile(filename: "01_test.c", directory: "build", checksumkind: CSK_MD5, checksum: "71667f9be91caf18efabadfadacd5332")
!10 = !{}
!11 = !{!"clang version 18.1.8 (https://github.com/llvm/llvm-project.git 3b5b5c1ec4a3095ab096dd780e84d7ab81f3d7ff)"}
!12 = distinct !DISubprogram(name: "cudaMalloc<float>", linkageName: "_ZL10cudaMallocIfE9cudaErrorPPT_m", scope: !13, file: !13, line: 679, type: !14, scopeLine: 683, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition | DISPFlagOptimized, unit: !8, templateParams: !10, retainedNodes: !10)
!13 = !DIFile(filename: "cuda_runtime.h", directory: "", checksumkind: CSK_MD5, checksum: "a4571448be7b4c49b92445823e9d2ad0")
!14 = distinct !DISubroutineType(types: !10)
!15 = distinct !DIAssignID()
!16 = !DILocalVariable(name: "devPtr", arg: 1, scope: !12, file: !13, line: 680, type: !17)
!17 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !18, size: 64)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!19 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!20 = !DILocation(line: 77, scope: !12)
!21 = distinct !DIAssignID()
!22 = !DILocalVariable(name: "size", arg: 2, scope: !12, file: !13, line: 681, type: !23)
!23 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !24, line: 18, baseType: !25)
!24 = !DIFile(filename: "/work/groups/da_sc/moduletree/packages-2023-08-24/.gcc/11.2.0/clang/18.1.8/lib/clang/18/include/__stddef_size_t.h", directory: "", checksumkind: CSK_MD5, checksum: "2c44e821a2b1951cde2eb0fb2e656867")
!25 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!26 = !{!27, !27, i64 0}
!27 = !{!"any pointer", !28, i64 0}
!28 = !{!"omnipotent char", !29, i64 0}
!29 = !{!"Simple C++ TBAA"}
!30 = distinct !DIAssignID()
!31 = !{!32, !32, i64 0}
!32 = !{!"long", !28, i64 0}
!33 = distinct !DIAssignID()
!34 = !DILocation(line: 684, column: 38, scope: !12)
!35 = !DILocation(line: 684, column: 46, scope: !12)
!36 = !DILocation(line: 684, column: 10, scope: !12)
!37 = !DILocation(line: 684, column: 3, scope: !12)
!38 = !DISubprogram(name: "cudaMalloc", scope: !39, file: !39, line: 4901, type: !40, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized)
!39 = !DIFile(filename: "cuda_runtime_api.h", directory: "", checksumkind: CSK_MD5, checksum: "dc23e9b7f5e15708c0fd23cd4ed34225")
!40 = !DISubroutineType(types: !10)
