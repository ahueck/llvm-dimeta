; RUN: %apply-verifier %s |& %filecheck %s
; CHECK-NOT: Assertion
; REQUIRES: llvm-18 || llvm-19

; CHECK: SourceLoc:
; CHECK-NEXT:   File:            '/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi/ompi/mpi/cxx/datatype_inln.h'
; CHECK-NEXT:   Function:        Create_contiguous
; CHECK-NEXT:   Line:            0
; CHECK-NEXT: Builtin:         false
; CHECK-NEXT: Type:
; CHECK-NEXT:   Compound:
; CHECK-NEXT:     Name:            Datatype
; CHECK-NEXT:     Identifier:      _ZTSN3MPI8DatatypeE
; CHECK-NEXT:     Type:            class
; CHECK-NEXT:     Extent:          16
; CHECK-NEXT:     Sizes:           [ 0 ]
; CHECK-NEXT:     Offsets:         [ 0 ]
; CHECK-NEXT:     Members:
; CHECK-NEXT:       - Name:            cxx_extra_states_lock
; CHECK-NEXT:         Builtin:         false
; CHECK-NEXT:         Type:
; CHECK-NEXT:           Compound:
; CHECK-NEXT:             Name:            opal_mutex_t
; CHECK-NEXT:             Identifier:      _ZTS12opal_mutex_t
; CHECK-NEXT:             Type:            struct
; CHECK-NEXT:             Extent:          0
; CHECK-NEXT:           Qualifiers:      [ static ]
; CHECK-NEXT:           ForwardDecl:     true
; CHECK-NEXT:   Qualifiers:      [ ptr, const ]


target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @_ZNK3MPI8Datatype17Create_contiguousEi() {
entry:
  %this.addr = alloca ptr, i32 0, align 8
  call void @llvm.dbg.declare(metadata ptr %this.addr, metadata !4, metadata !DIExpression()), !dbg !18
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

attributes #0 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !1, producer: "clang version 18.1.8", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !2, imports: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "typeart/test/script/05_wrapper_mpicxx.cpp", directory: "typeart", checksumkind: CSK_MD5, checksum: "c3331e3bb3d69c748d930e271bab2933")
!2 = !{}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !DILocalVariable(name: "this", arg: 1, scope: !5, type: !16, flags: DIFlagArtificial | DIFlagObjectPointer)
!5 = distinct !DISubprogram(name: "Create_contiguous", linkageName: "_ZNK3MPI8Datatype17Create_contiguousEi", scope: !7, file: !6, line: 29, type: !14, scopeLine: 30, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, declaration: !15, retainedNodes: !2)
!6 = !DIFile(filename: "/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi/ompi/mpi/cxx/datatype_inln.h", directory: "", checksumkind: CSK_MD5, checksum: "ca341002155830b958f1367079263e60")
!7 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "Datatype", scope: !9, file: !8, line: 24, size: 128, flags: DIFlagTypePassByReference | DIFlagNonTrivial, elements: !10, vtableHolder: !7, identifier: "_ZTSN3MPI8DatatypeE")
!8 = !DIFile(filename: "/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi/ompi/mpi/cxx/datatype.h", directory: "", checksumkind: CSK_MD5, checksum: "d913c24e11a2093e97826064a436b07e")
!9 = !DINamespace(name: "MPI", scope: null)
!10 = !{!11}
!11 = !DIDerivedType(tag: DW_TAG_variable, name: "cxx_extra_states_lock", scope: !7, file: !8, line: 257, baseType: !12, flags: DIFlagPublic | DIFlagStaticMember)
!12 = !DICompositeType(tag: DW_TAG_structure_type, name: "opal_mutex_t", file: !13, line: 97, flags: DIFlagFwdDecl | DIFlagNonTrivial, identifier: "_ZTS12opal_mutex_t")
!13 = !DIFile(filename: "/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi/ompi/mpi/cxx/mpicxx.h", directory: "", checksumkind: CSK_MD5, checksum: "73d4b420d7e4bd422e8c829e20bee55b")
!14 = distinct !DISubroutineType(types: !2)
!15 = !DISubprogram(name: "Create_contiguous", linkageName: "_ZNK3MPI8Datatype17Create_contiguousEi", scope: !7, file: !8, line: 112, type: !14, scopeLine: 112, containingType: !7, virtualIndex: 2, flags: DIFlagPublic | DIFlagPrototyped, spFlags: DISPFlagVirtual | DISPFlagOptimized)
!16 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
!17 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !7)
!18 = !DILocation(line: 0, scope: !5)
