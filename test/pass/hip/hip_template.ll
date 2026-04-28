; RUN: %apply-verifier %s |& %filecheck %s
; REQUIRES: llvm-18 || llvm-19 || llvm-20 || llvm-21 || llvm-22

; CHECK:   Function:        'hipMalloc<float>'
; CHECK-NEXT:   Line:            {{[0-9]+}}
; CHECK-NEXT: Builtin:         true
; CHECK-NEXT: Type:
; CHECK-NEXT:   Fundamental:     { Name: float, Extent: 4, Encoding: float }
; CHECK-NEXT:   Qualifiers:      [ ptr, ptr ]

source_filename = "01_hipmalloc.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@__hip_cuid_b9496d44041ec330 = global i8 0
@llvm.compiler.used = appending global [1 x ptr] [ptr @__hip_cuid_b9496d44041ec330], section "llvm.metadata"

; Function Attrs: inlinehint mustprogress uwtable
define internal noundef i32 @_ZL9hipMallocIfE10hipError_tPPT_m(ptr noundef %0, i64 noundef %1) #0 !dbg !7 {
  %3 = alloca ptr, align 8
  %4 = alloca i64, align 8
  store ptr %0, ptr %3, align 8, !tbaa !10
    #dbg_declare(ptr %3, !14, !DIExpression(), !18)
  store i64 %1, ptr %4, align 8, !tbaa !19
    #dbg_declare(ptr %4, !21, !DIExpression(), !25)
  %5 = load ptr, ptr %3, align 8, !dbg !26, !tbaa !10
  %6 = load i64, ptr %4, align 8, !dbg !27, !tbaa !19
  %7 = call i32 @hipMalloc(ptr noundef %5, i64 noundef %6), !dbg !28
  ret i32 %7, !dbg !29
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr captures(none)) #1

declare !dbg !30 dso_local i32 @hipMalloc(ptr noundef, i64 noundef) #2

attributes #0 = { inlinehint mustprogress uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }


!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !1, producer: "AMD clang version 19.0.0git (https://github.com/RadeonOpenCompute/llvm-project roc-6.4.4 25224 d366fa84f3fdcbd4b10847ebd5db572ae12a34fb)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !2, imports: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "01_hipmalloc.c", directory: "")
!2 = !{}
!3 = !{i32 7, !"Dwarf Version", i32 5}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"uwtable", i32 2}
!7 = distinct !DISubprogram(name: "hipMalloc<float>", linkageName: "_ZL9hipMallocIfE10hipError_tPPT_m", scope: !8, file: !8, line: 9761, type: !9, scopeLine: 9761, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition | DISPFlagOptimized, unit: !0, templateParams: !2, retainedNodes: !2)
!8 = !DIFile(filename: "hip_runtime_api.h", directory: "")
!9 = distinct !DISubroutineType(types: !2)
!10 = !{!11, !11, i64 0}
!11 = !{!"any pointer", !12, i64 0}
!12 = !{!"omnipotent char", !13, i64 0}
!13 = !{!"Simple C++ TBAA"}
!14 = !DILocalVariable(name: "devPtr", arg: 1, scope: !7, file: !8, line: 9761, type: !15)
!15 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !16, size: 64)
!16 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
!17 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!18 = !DILocation(line: 9761, column: 40, scope: !7)
!19 = !{!20, !20, i64 0}
!20 = !{!"long", !12, i64 0}
!21 = !DILocalVariable(name: "size", arg: 2, scope: !7, file: !8, line: 9761, type: !22)
!22 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !23, line: 18, baseType: !24)
!23 = !DIFile(filename: "__stddef_size_t.h", directory: "")
!24 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!25 = !DILocation(line: 9761, column: 55, scope: !7)
!26 = !DILocation(line: 9762, column: 30, scope: !7)
!27 = !DILocation(line: 9762, column: 38, scope: !7)
!28 = !DILocation(line: 9762, column: 12, scope: !7)
!29 = !DILocation(line: 9762, column: 5, scope: !7)
!30 = !DISubprogram(name: "hipMalloc", scope: !8, file: !8, line: 3446, type: !31, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized)
!31 = !DISubroutineType(types: !2)
