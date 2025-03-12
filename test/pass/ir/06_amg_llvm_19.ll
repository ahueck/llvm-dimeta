; RUN: %apply-verifier %s |& %filecheck %s
; CHECK-NOT: Assertion
; REQUIRES: llvm-19

target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define i32 @hypre_SStructPMatvecSetup() {
entry:
  %smatvec_data = alloca ptr, i32 0, align 8
    #dbg_declare(ptr %smatvec_data, !4, !DIExpression(), !10)
  %call18 = call ptr @malloc()
  %0 = load ptr, ptr %smatvec_data, align 8
  %arrayidx = getelementptr ptr, ptr %0, i64 0
  store ptr %call18, ptr %arrayidx, align 8
  ret i32 0
}

declare ptr @malloc()

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3}

!0 = distinct !DICompileUnit(language: DW_LANG_C89, file: !1, producer: "clang version 19.1.6", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !2, globals: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "sstruct_matvec.c", directory: "amg2013/sstruct_mv", checksumkind: CSK_MD5, checksum: "c996cadcade0f7e11c149eb0894dfcac")
!2 = !{}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !DILocalVariable(name: "smatvec_data", scope: !5, file: !1, line: 66, type: !7)
!5 = distinct !DISubprogram(name: "hypre_SStructPMatvecSetup", scope: !1, file: !1, line: 59, type: !6, scopeLine: 62, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !2)
!6 = distinct !DISubroutineType(types: !2)
!7 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !8, size: 64)
!8 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !9, size: 64)
!9 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!10 = !DILocation(line: 66, column: 32, scope: !5)
