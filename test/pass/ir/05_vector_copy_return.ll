; RUN: %apply-verifier %s |& %filecheck %s
; CHECK-NOT: Assertion
; REQUIRES: llvm-18 || llvm-19

target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::vector" = type { %"struct.std::_Vector_base" }
%"struct.std::_Vector_base" = type { %"struct.std::_Vector_base<int *, std::allocator<int *>>::_Vector_impl" }
%"struct.std::_Vector_base<int *, std::allocator<int *>>::_Vector_impl" = type { %"struct.std::_Vector_base<int *, std::allocator<int *>>::_Vector_impl_data" }
%"struct.std::_Vector_base<int *, std::allocator<int *>>::_Vector_impl_data" = type { ptr, ptr, ptr }

define void @_Z3foov(ptr sret(%"class.std::vector") %agg.result) personality ptr null !dbg !4 {
entry:
  br label %_ZNSt12_Vector_baseIPiSaIS0_EEC2EmRKS1_.exit.i

_ZNSt15__new_allocatorIPiE8allocateEmPKv.exit.i.i.i.i: ; No predecessors!
  %call5.i.i.i2.i6.i = call ptr @_Znwm()
  br label %_ZNSt12_Vector_baseIPiSaIS0_EEC2EmRKS1_.exit.i

_ZNSt12_Vector_baseIPiSaIS0_EEC2EmRKS1_.exit.i:   ; preds = %_ZNSt15__new_allocatorIPiE8allocateEmPKv.exit.i.i.i.i, %entry
  %cond.i.i.i.i = phi ptr [ null, %entry ], [ %call5.i.i.i2.i6.i, %_ZNSt15__new_allocatorIPiE8allocateEmPKv.exit.i.i.i.i ]
  %_M_finish.i.i.i = getelementptr i8, ptr %agg.result, i64 8
  store ptr %cond.i.i.i.i, ptr %_M_finish.i.i.i, align 8
  ret void
}

declare ptr @_Znwm()

; uselistorder directives
uselistorder ptr null, { 1, 2, 3, 0 }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !1, producer: "clang version 19.1.6", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !2, globals: !2, imports: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "heap_lhs_function_opt_nofwd.cpp", directory: "dimeta/build_19", checksumkind: CSK_MD5, checksum: "49cf2cd7566f3ad7be07978f77374c6e")
!2 = !{}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = distinct !DISubprogram(name: "foo", linkageName: "_Z3foov", scope: !5, file: !5, line: 8, type: !6, scopeLine: 8, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0)
!5 = !DIFile(filename: "test/pass/cpp/heap_lhs_function_opt_nofwd.cpp", directory: "dimeta", checksumkind: CSK_MD5, checksum: "49cf2cd7566f3ad7be07978f77374c6e")
!6 = !DISubroutineType(types: !7)
!7 = !{!8}
!8 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "vector<int *, std::allocator<int *> >", scope: !10, file: !9, line: 428, size: 192, flags: DIFlagTypePassByReference | DIFlagNonTrivial, elements: !11, templateParams: !2, identifier: "_ZTSSt6vectorIPiSaIS0_EE")
!9 = !DIFile(filename: "stl_vector.h", directory: "", checksumkind: CSK_MD5, checksum: "514164964ac06e2061e9e779d8cf420e")
!10 = !DINamespace(name: "std", scope: null)
!11 = !{!12}
!12 = !DIDerivedType(tag: DW_TAG_inheritance, scope: !8, baseType: !13, flags: DIFlagProtected, extraData: i32 0)
!13 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "_Vector_base<int *, std::allocator<int *> >", scope: !10, file: !9, line: 85, size: 192, flags: DIFlagTypePassByReference | DIFlagNonTrivial, elements: !2, templateParams: !2, identifier: "_ZTSSt12_Vector_baseIPiSaIS0_EE")
