; RUN: %apply-verifier %s | %filecheck %s
; CHECK-NOT: Stack dump:

; REQUIRES: llvm-14

source_filename = "par_multi_interp.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.hypre_ParCSRMatrix = type { %struct.ompi_communicator_t*, i64, i64, i64, i64, i64, i64, %struct.hypre_CSRMatrix*, %struct.hypre_CSRMatrix*, i64*, i64*, i64*, %struct.hypre_ParCSRCommPkg*, %struct.hypre_ParCSRCommPkg*, i32, i32, i32, i64, double, i64*, double*, i32, %struct.hypre_IJAssumedPart* }
%struct.ompi_communicator_t = type opaque
%struct.hypre_CSRMatrix = type { double*, i32*, i32*, i32, i32, i32, i32*, i32, i32, double* }
%struct.hypre_ParCSRCommPkg = type { %struct.ompi_communicator_t*, i32, i32*, i32*, i32*, i32, i32*, i32*, %struct.ompi_datatype_t**, %struct.ompi_datatype_t** }
%struct.ompi_datatype_t = type opaque
%struct.hypre_IJAssumedPart = type { i32, i64, i64, i32, i32*, i64*, i64*, i32* }

; Function Attrs: nounwind uwtable
define i32 @hypre_BoomerAMGBuildMultipass(%struct.hypre_ParCSRMatrix* noundef %A, i32* noundef %CF_marker, i32* noundef %dof_func, i32 noundef %debug_flag, double noundef %trunc_factor, i32 noundef %P_max_elmts, i32 noundef %weight_option, i32* noundef %col_offd_S_to_A) #0 {
entry:
  %pass_pointer = alloca i32*, i32 0, align 8
  %0 = load %struct.ompi_communicator_t*, %struct.ompi_communicator_t** null, align 8
  call void @llvm.dbg.value(metadata %struct.ompi_communicator_t* %0, metadata !46, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata %struct.hypre_ParCSRCommPkg* undef, metadata !118, metadata !DIExpression()), !dbg !273
  %diag = getelementptr inbounds %struct.hypre_ParCSRMatrix, %struct.hypre_ParCSRMatrix* %A, i64 0, i32 7, !dbg !274
  call void @llvm.dbg.value(metadata %struct.hypre_CSRMatrix* undef, metadata !134, metadata !DIExpression()), !dbg !273
  %1 = load i64*, i64** null, align 8
  call void @llvm.dbg.value(metadata i64* %1, metadata !142, metadata !DIExpression()), !dbg !273
  %2 = load i32, i32* null, align 4
  call void @llvm.dbg.value(metadata i32 %2, metadata !143, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata %struct.hypre_CSRMatrix* undef, metadata !144, metadata !DIExpression()), !dbg !273
  %3 = bitcast i32** null to i8*
  call void @llvm.dbg.value(metadata i32* undef, metadata !146, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata %struct.hypre_CSRMatrix* undef, metadata !147, metadata !DIExpression()), !dbg !273
  %4 = bitcast i32** null to i8*
  store i32* null, i32** null, align 8, !dbg !275
  call void @llvm.dbg.value(metadata i32* null, metadata !149, metadata !DIExpression()), !dbg !273
  store i32* null, i32** null, align 8, !dbg !276
  %5 = bitcast i32* null to i8*, !dbg !277
  call void @llvm.dbg.value(metadata i32* null, metadata !176, metadata !DIExpression()), !dbg !273
  ret i32 0

if.end2559:                                       ; No predecessors!
  call void @llvm.dbg.value(metadata i32 undef, metadata !257, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata double* undef, metadata !217, metadata !DIExpression()), !dbg !273
  %6 = load i32*, i32** %pass_pointer, align 8
  %arrayidx2561 = getelementptr inbounds i32, i32* %6, i64 undef
  %7 = load i32, i32* %arrayidx2561, align 4
  call void @llvm.dbg.value(metadata i32 %7, metadata !230, metadata !DIExpression()), !dbg !273
  br label %for.body2568.lr.ph

for.body2568.lr.ph:                               ; preds = %if.end2559
  br label %for.body2568

for.body2568:                                     ; preds = %for.body2568.lr.ph
  call void @llvm.dbg.value(metadata double undef, metadata !226, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata i32 %7, metadata !230, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata i32* undef, metadata !183, metadata !DIExpression()), !dbg !273
  %idxprom2569 = sext i32 %7 to i64
  %arrayidx2570 = getelementptr inbounds i32, i32* undef, i64 %idxprom2569
  %8 = load i32, i32* %arrayidx2570, align 4
  %idxprom2571 = sext i32 %8 to i64
  ret i32 0

for.end2674:                                      ; No predecessors!
  call void @llvm.dbg.value(metadata i32* undef, metadata !136, metadata !DIExpression()), !dbg !273
  %arrayidx2676 = getelementptr inbounds i32, i32* undef, i64 %idxprom2571
  %9 = load i32, i32* %arrayidx2676, align 4
  call void @llvm.dbg.value(metadata i32 %9, metadata !232, metadata !DIExpression(DW_OP_plus_uconst, 1, DW_OP_stack_value)), !dbg !273
  %j13.305221 = add i32 %9, 1
  br i1 undef, label %for.body2684.lr.ph, label %for.end2791

for.body2684.lr.ph:                               ; preds = %for.end2674
  %10 = sext i32 %j13.305221 to i64
  br label %for.body2684

for.body2684:                                     ; preds = %for.inc2789, %for.body2684.lr.ph
  %indvars.iv5609 = phi i64 [ %10, %for.body2684.lr.ph ], [ 0, %for.inc2789 ]
  %alfa.45231 = phi double [ 0.000000e+00, %for.body2684.lr.ph ], [ %alfa.7, %for.inc2789 ]
  %arrayidx2686 = getelementptr inbounds i32, i32* undef, i64 %indvars.iv5609
  %11 = load i32, i32* %arrayidx2686, align 4
  call void @llvm.dbg.value(metadata i32 %11, metadata !233, metadata !DIExpression()), !dbg !273
  %idxprom2687 = sext i32 %11 to i64
  br label %if.then2691

if.then2691:                                      ; preds = %for.body2684
  call void @llvm.dbg.value(metadata i32* undef, metadata !157, metadata !DIExpression()), !dbg !273
  %arrayidx2693 = getelementptr inbounds i32, i32* undef, i64 %idxprom2687
  %12 = load i32, i32* %arrayidx2693, align 4
  call void @llvm.dbg.value(metadata i32 %12, metadata !234, metadata !DIExpression()), !dbg !273
  br i1 false, label %for.body2700.lr.ph, label %for.end2724

for.body2700.lr.ph:                               ; preds = %if.then2691
  %13 = sext i32 %12 to i64
  br label %for.body2700

for.body2700:                                     ; preds = %for.inc2722, %for.body2700.lr.ph
  %indvars.iv5599 = phi i64 [ %13, %for.body2700.lr.ph ], [ 0, %for.inc2722 ]
  call void @llvm.dbg.value(metadata i32 undef, metadata !235, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata double* undef, metadata !135, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata double* undef, metadata !156, metadata !DIExpression()), !dbg !273
  %arrayidx2706 = getelementptr inbounds double, double* undef, i64 %indvars.iv5599
  %14 = load double, double* %arrayidx2706, align 8
  %mul2707 = fmul double 0.000000e+00, %14
  call void @llvm.dbg.value(metadata double %mul2707, metadata !226, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata i32* undef, metadata !181, metadata !DIExpression()), !dbg !273
  br i1 false, label %if.then2715, label %if.else2718

if.then2715:                                      ; preds = %for.body2700
  %add2717 = fadd double 0.000000e+00, %mul2707
  call void @llvm.dbg.value(metadata double %add2717, metadata !224, metadata !DIExpression()), !dbg !273
  br label %for.inc2722

if.else2718:                                      ; preds = %for.body2700
  %add2719 = fadd double 0.000000e+00, %mul2707
  call void @llvm.dbg.value(metadata double %add2719, metadata !221, metadata !DIExpression()), !dbg !273
  %add2720 = fadd double 0.000000e+00, %mul2707
  br label %for.inc2722

for.inc2722:                                      ; preds = %if.else2718, %if.then2715
  %sum_N_pos.6 = phi double [ 0.000000e+00, %if.then2715 ], [ %add2720, %if.else2718 ]
  %sum_N_neg.6 = phi double [ 0.000000e+00, %if.then2715 ], [ 0.000000e+00, %if.else2718 ]
  call void @llvm.dbg.value(metadata double %sum_N_neg.6, metadata !224, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata i64 undef, metadata !234, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata double %mul2707, metadata !226, metadata !DIExpression()), !dbg !273
  br i1 false, label %for.end2724, label %for.body2700

for.end2724:                                      ; preds = %for.inc2722, %if.then2691
  %sum_N_pos.5.lcssa = phi double [ 0.000000e+00, %if.then2691 ], [ %sum_N_pos.6, %for.inc2722 ]
  %sum_N_neg.5.lcssa = phi double [ 0.000000e+00, %if.then2691 ], [ %add2717, %for.inc2722 ]
  br i1 false, label %for.body2733.lr.ph, label %for.inc2789

for.body2733.lr.ph:                               ; preds = %for.end2724
  br label %for.body2733

for.body2733:                                     ; preds = %for.inc2755, %for.body2733.lr.ph
  %sum_N_neg.75214 = phi double [ 0.000000e+00, %for.body2733.lr.ph ], [ 0.000000e+00, %for.inc2755 ]
  %sum_N_pos.75213 = phi double [ %sum_N_pos.5.lcssa, %for.body2733.lr.ph ], [ 0.000000e+00, %for.inc2755 ]
  br i1 false, label %if.then2748, label %if.else2751

if.then2748:                                      ; preds = %for.body2733
  call void @llvm.dbg.value(metadata double undef, metadata !222, metadata !DIExpression()), !dbg !273
  %add2750 = fadd double 0.000000e+00, 0.000000e+00
  call void @llvm.dbg.value(metadata double %add2750, metadata !224, metadata !DIExpression()), !dbg !273
  br label %for.inc2755

if.else2751:                                      ; preds = %for.body2733
  call void @llvm.dbg.value(metadata double undef, metadata !221, metadata !DIExpression()), !dbg !273
  %add2753 = fadd double %sum_N_pos.75213, 0.000000e+00
  br label %for.inc2755

for.inc2755:                                      ; preds = %if.else2751, %if.then2748
  %sum_N_pos.8 = phi double [ %sum_N_pos.75213, %if.then2748 ], [ %add2753, %if.else2751 ]
  %sum_N_neg.8 = phi double [ %sum_N_neg.5.lcssa, %if.then2748 ], [ 0.000000e+00, %if.else2751 ]
  br i1 false, label %for.inc2789, label %for.body2733

for.inc2789:                                      ; preds = %for.inc2755, %for.end2724
  %sum_N_pos.9 = phi double [ %sum_N_pos.5.lcssa, %for.end2724 ], [ %sum_N_pos.8, %for.inc2755 ]
  %sum_N_neg.9 = phi double [ %sum_N_neg.5.lcssa, %for.end2724 ], [ %sum_N_neg.5.lcssa, %for.inc2755 ]
  %alfa.7 = phi double [ 0.000000e+00, %for.end2724 ], [ 0.000000e+00, %for.inc2755 ]
  call void @llvm.dbg.value(metadata double %alfa.7, metadata !226, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata double %sum_N_neg.9, metadata !224, metadata !DIExpression()), !dbg !273
  call void @llvm.dbg.value(metadata double %sum_N_pos.9, metadata !223, metadata !DIExpression()), !dbg !273
  br i1 false, label %for.end2791, label %for.body2684

for.end2791:                                      ; preds = %for.inc2789, %for.end2674
  %sum_N_pos.4.lcssa = phi double [ 0.000000e+00, %for.end2674 ], [ %sum_N_pos.9, %for.inc2789 ]
  %sum_N_neg.4.lcssa = phi double [ 0.000000e+00, %for.end2674 ], [ %sum_N_neg.9, %for.inc2789 ]
  br label %for.body2800.lr.ph

for.body2800.lr.ph:                               ; preds = %for.end2791
  br label %for.body2800

for.body2800:                                     ; preds = %for.inc2898, %for.body2800.lr.ph
  %sum_N_neg.105260 = phi double [ %sum_N_neg.9, %for.body2800.lr.ph ], [ 0.000000e+00, %for.inc2898 ]
  %sum_N_pos.105257 = phi double [ %sum_N_pos.4.lcssa, %for.body2800.lr.ph ], [ 0.000000e+00, %for.inc2898 ]
  ret i32 0

if.else2867:                                      ; No predecessors!
  br label %for.inc2898

lor.lhs.false2875:                                ; No predecessors!
  call void @llvm.dbg.value(metadata i32* undef, metadata !111, metadata !DIExpression()), !dbg !273
  br label %for.inc2898

if.then2887:                                      ; No predecessors!
  %add2890 = fadd double 0.000000e+00, 0.000000e+00
  call void @llvm.dbg.value(metadata double %add2890, metadata !224, metadata !DIExpression()), !dbg !273
  br label %for.inc2898

if.else2891:                                      ; No predecessors!
  %add2894 = fadd double %sum_N_pos.105257, 0.000000e+00
  call void @llvm.dbg.value(metadata double %add2894, metadata !223, metadata !DIExpression()), !dbg !273
  br label %for.inc2898

for.inc2898:                                      ; preds = %if.else2891, %if.then2887, %lor.lhs.false2875, %if.else2867
  %sum_N_pos.13 = phi double [ %sum_N_pos.105257, %if.then2887 ], [ 0.000000e+00, %if.else2891 ], [ %sum_N_pos.105257, %lor.lhs.false2875 ], [ %sum_N_pos.105257, %if.else2867 ]
  %sum_N_neg.13 = phi double [ 0.000000e+00, %if.then2887 ], [ %sum_N_neg.4.lcssa, %if.else2891 ], [ %sum_N_neg.4.lcssa, %lor.lhs.false2875 ], [ %sum_N_neg.4.lcssa, %if.else2867 ]
  br label %for.body2800

; uselistorder directives
  uselistorder double %sum_N_neg.5.lcssa, { 0, 2, 1 }
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #2

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #2

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

; Function Attrs: inaccessiblememonly nofree nosync nounwind willreturn
declare void @llvm.assume(i1 noundef) #3

; Function Attrs: argmemonly nofree nounwind willreturn writeonly
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #4

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare i32 @llvm.smax.i32(i32, i32) #1

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare i64 @llvm.smax.i64(i64, i64) #1

; uselistorder directives
uselistorder double 0.000000e+00, { 5, 6, 7, 8, 0, 9, 10, 11, 12, 13, 14, 1, 15, 16, 2, 17, 18, 3, 19, 20, 21, 4, 22, 23, 24, 25, 26, 27 }
uselistorder void (metadata, metadata, metadata)* @llvm.dbg.value, { 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 }

attributes #0 = { nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { argmemonly nofree nosync nounwind willreturn }
attributes #3 = { inaccessiblememonly nofree nosync nounwind willreturn }
attributes #4 = { argmemonly nofree nounwind willreturn writeonly }


!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!41, !42, !43, !44, !45}
!0 = distinct !DICompileUnit(language: DW_LANG_C89, file: !1, producer: "clang version 14.0.6", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "par_multi_interp.c", directory: "parcsr_ls")
!2 = !{!3, !7, !8, !9, !11, !12, !13, !15, !18, !19, !20, !10, !39}
!3 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Datatype", file: !4, line: 424, baseType: !5)
!4 = !DIFile(filename: "mpi.h", directory: "")
!5 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!6 = !DICompositeType(tag: DW_TAG_structure_type, name: "ompi_datatype_t", file: !4, line: 424, flags: DIFlagFwdDecl)
!7 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!8 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!9 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !10, size: 64)
!10 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!14 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!15 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Op", file: !4, line: 429, baseType: !16)
!16 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
!17 = !DICompositeType(tag: DW_TAG_structure_type, name: "ompi_op_t", file: !4, line: 429, flags: DIFlagFwdDecl)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !9, size: 64)
!19 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!20 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !21, size: 64)
!21 = !DIDerivedType(tag: DW_TAG_typedef, name: "hypre_ParCSRCommPkg", file: !22, line: 86, baseType: !23)
!22 = !DIFile(filename: "parcsr_mv.h", directory: "parcsr_ls")
!23 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !22, line: 69, size: 640, elements: !24)
!24 = !{!25, !29, !30, !31, !32, !33, !34, !35, !36, !38}
!25 = !DIDerivedType(tag: DW_TAG_member, name: "comm", scope: !23, file: !22, line: 71, baseType: !26, size: 64)
!26 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Comm", file: !4, line: 423, baseType: !27)
!27 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !28, size: 64)
!28 = !DICompositeType(tag: DW_TAG_structure_type, name: "ompi_communicator_t", file: !4, line: 423, flags: DIFlagFwdDecl)
!29 = !DIDerivedType(tag: DW_TAG_member, name: "num_sends", scope: !23, file: !22, line: 73, baseType: !10, size: 32, offset: 64)
!30 = !DIDerivedType(tag: DW_TAG_member, name: "send_procs", scope: !23, file: !22, line: 74, baseType: !9, size: 64, offset: 128)
!31 = !DIDerivedType(tag: DW_TAG_member, name: "send_map_starts", scope: !23, file: !22, line: 75, baseType: !9, size: 64, offset: 192)
!32 = !DIDerivedType(tag: DW_TAG_member, name: "send_map_elmts", scope: !23, file: !22, line: 76, baseType: !9, size: 64, offset: 256)
!33 = !DIDerivedType(tag: DW_TAG_member, name: "num_recvs", scope: !23, file: !22, line: 78, baseType: !10, size: 32, offset: 320)
!34 = !DIDerivedType(tag: DW_TAG_member, name: "recv_procs", scope: !23, file: !22, line: 79, baseType: !9, size: 64, offset: 384)
!35 = !DIDerivedType(tag: DW_TAG_member, name: "recv_vec_starts", scope: !23, file: !22, line: 80, baseType: !9, size: 64, offset: 448)
!36 = !DIDerivedType(tag: DW_TAG_member, name: "send_mpi_types", scope: !23, file: !22, line: 83, baseType: !37, size: 64, offset: 512)
!37 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !3, size: 64)
!38 = !DIDerivedType(tag: DW_TAG_member, name: "recv_mpi_types", scope: !23, file: !22, line: 84, baseType: !37, size: 64, offset: 576)
!39 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !40, size: 64)
!40 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!41 = !{i32 7, !"Dwarf Version", i32 5}
!42 = !{i32 2, !"Debug Info Version", i32 3}
!43 = !{i32 1, !"wchar_size", i32 4}
!44 = !{i32 7, !"openmp", i32 50}
!45 = !{i32 7, !"uwtable", i32 1}
!46 = !DILocalVariable(name: "comm", scope: !47, file: !1, line: 36, type: !26)
!47 = distinct !DISubprogram(name: "hypre_BoomerAMGBuildMultipass", scope: !1, file: !1, line: 23, type: !48, scopeLine: 35, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !105)
!48 = !DISubroutineType(types: !49)
!49 = !{!10, !50, !9, !50, !11, !10, !9, !10, !40, !10, !10, !9, !104}
!50 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !51, size: 64)
!51 = !DIDerivedType(tag: DW_TAG_typedef, name: "hypre_ParCSRMatrix", file: !22, line: 313, baseType: !52)
!52 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !22, line: 264, size: 1408, elements: !53)
!53 = !{!54, !55, !56, !57, !58, !59, !60, !61, !77, !78, !79, !80, !81, !82, !83, !84, !85, !86, !87, !88, !89, !90, !91}
!54 = !DIDerivedType(tag: DW_TAG_member, name: "comm", scope: !52, file: !22, line: 266, baseType: !26, size: 64)
!55 = !DIDerivedType(tag: DW_TAG_member, name: "global_num_rows", scope: !52, file: !22, line: 268, baseType: !12, size: 64, offset: 64)
!56 = !DIDerivedType(tag: DW_TAG_member, name: "global_num_cols", scope: !52, file: !22, line: 269, baseType: !12, size: 64, offset: 128)
!57 = !DIDerivedType(tag: DW_TAG_member, name: "first_row_index", scope: !52, file: !22, line: 270, baseType: !12, size: 64, offset: 192)
!58 = !DIDerivedType(tag: DW_TAG_member, name: "first_col_diag", scope: !52, file: !22, line: 271, baseType: !12, size: 64, offset: 256)
!59 = !DIDerivedType(tag: DW_TAG_member, name: "last_row_index", scope: !52, file: !22, line: 274, baseType: !12, size: 64, offset: 320)
!60 = !DIDerivedType(tag: DW_TAG_member, name: "last_col_diag", scope: !52, file: !22, line: 275, baseType: !12, size: 64, offset: 384)
!61 = !DIDerivedType(tag: DW_TAG_member, name: "diag", scope: !52, file: !22, line: 277, baseType: !62, size: 64, offset: 448)
!62 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !63, size: 64)
!63 = !DIDerivedType(tag: DW_TAG_typedef, name: "hypre_CSRMatrix", file: !64, line: 73, baseType: !65)
!64 = !DIFile(filename: "seq_mv.h", directory: "parcsr_ls")
!65 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !64, line: 54, size: 512, elements: !66)
!66 = !{!67, !68, !69, !70, !71, !72, !73, !74, !75, !76}
!67 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !65, file: !64, line: 56, baseType: !39, size: 64)
!68 = !DIDerivedType(tag: DW_TAG_member, name: "i", scope: !65, file: !64, line: 57, baseType: !9, size: 64, offset: 64)
!69 = !DIDerivedType(tag: DW_TAG_member, name: "j", scope: !65, file: !64, line: 58, baseType: !9, size: 64, offset: 128)
!70 = !DIDerivedType(tag: DW_TAG_member, name: "num_rows", scope: !65, file: !64, line: 59, baseType: !10, size: 32, offset: 192)
!71 = !DIDerivedType(tag: DW_TAG_member, name: "num_cols", scope: !65, file: !64, line: 60, baseType: !10, size: 32, offset: 224)
!72 = !DIDerivedType(tag: DW_TAG_member, name: "num_nonzeros", scope: !65, file: !64, line: 61, baseType: !10, size: 32, offset: 256)
!73 = !DIDerivedType(tag: DW_TAG_member, name: "rownnz", scope: !65, file: !64, line: 64, baseType: !9, size: 64, offset: 320)
!74 = !DIDerivedType(tag: DW_TAG_member, name: "num_rownnz", scope: !65, file: !64, line: 65, baseType: !10, size: 32, offset: 384)
!75 = !DIDerivedType(tag: DW_TAG_member, name: "owns_data", scope: !65, file: !64, line: 68, baseType: !10, size: 32, offset: 416)
!76 = !DIDerivedType(tag: DW_TAG_member, name: "expand_data", scope: !65, file: !64, line: 71, baseType: !39, size: 64, offset: 448)
!77 = !DIDerivedType(tag: DW_TAG_member, name: "offd", scope: !52, file: !22, line: 278, baseType: !62, size: 64, offset: 512)
!78 = !DIDerivedType(tag: DW_TAG_member, name: "col_map_offd", scope: !52, file: !22, line: 279, baseType: !11, size: 64, offset: 576)
!79 = !DIDerivedType(tag: DW_TAG_member, name: "row_starts", scope: !52, file: !22, line: 281, baseType: !11, size: 64, offset: 640)
!80 = !DIDerivedType(tag: DW_TAG_member, name: "col_starts", scope: !52, file: !22, line: 286, baseType: !11, size: 64, offset: 704)
!81 = !DIDerivedType(tag: DW_TAG_member, name: "comm_pkg", scope: !52, file: !22, line: 292, baseType: !20, size: 64, offset: 768)
!82 = !DIDerivedType(tag: DW_TAG_member, name: "comm_pkgT", scope: !52, file: !22, line: 293, baseType: !20, size: 64, offset: 832)
!83 = !DIDerivedType(tag: DW_TAG_member, name: "owns_data", scope: !52, file: !22, line: 296, baseType: !10, size: 32, offset: 896)
!84 = !DIDerivedType(tag: DW_TAG_member, name: "owns_row_starts", scope: !52, file: !22, line: 298, baseType: !10, size: 32, offset: 928)
!85 = !DIDerivedType(tag: DW_TAG_member, name: "owns_col_starts", scope: !52, file: !22, line: 299, baseType: !10, size: 32, offset: 960)
!86 = !DIDerivedType(tag: DW_TAG_member, name: "num_nonzeros", scope: !52, file: !22, line: 301, baseType: !12, size: 64, offset: 1024)
!87 = !DIDerivedType(tag: DW_TAG_member, name: "d_num_nonzeros", scope: !52, file: !22, line: 302, baseType: !40, size: 64, offset: 1088)
!88 = !DIDerivedType(tag: DW_TAG_member, name: "rowindices", scope: !52, file: !22, line: 305, baseType: !11, size: 64, offset: 1152)
!89 = !DIDerivedType(tag: DW_TAG_member, name: "rowvalues", scope: !52, file: !22, line: 306, baseType: !39, size: 64, offset: 1216)
!90 = !DIDerivedType(tag: DW_TAG_member, name: "getrowactive", scope: !52, file: !22, line: 307, baseType: !10, size: 32, offset: 1280)
!91 = !DIDerivedType(tag: DW_TAG_member, name: "assumed_partition", scope: !52, file: !22, line: 309, baseType: !92, size: 64, offset: 1344)
!92 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !93, size: 64)
!93 = !DIDerivedType(tag: DW_TAG_typedef, name: "hypre_IJAssumedPart", file: !22, line: 155, baseType: !94)
!94 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !22, line: 145, size: 512, elements: !95)
!95 = !{!96, !97, !98, !99, !100, !101, !102, !103}
!96 = !DIDerivedType(tag: DW_TAG_member, name: "length", scope: !94, file: !22, line: 147, baseType: !10, size: 32)
!97 = !DIDerivedType(tag: DW_TAG_member, name: "row_start", scope: !94, file: !22, line: 148, baseType: !12, size: 64, offset: 64)
!98 = !DIDerivedType(tag: DW_TAG_member, name: "row_end", scope: !94, file: !22, line: 149, baseType: !12, size: 64, offset: 128)
!99 = !DIDerivedType(tag: DW_TAG_member, name: "storage_length", scope: !94, file: !22, line: 150, baseType: !10, size: 32, offset: 192)
!100 = !DIDerivedType(tag: DW_TAG_member, name: "proc_list", scope: !94, file: !22, line: 151, baseType: !9, size: 64, offset: 256)
!101 = !DIDerivedType(tag: DW_TAG_member, name: "row_start_list", scope: !94, file: !22, line: 152, baseType: !11, size: 64, offset: 320)
!102 = !DIDerivedType(tag: DW_TAG_member, name: "row_end_list", scope: !94, file: !22, line: 153, baseType: !11, size: 64, offset: 384)
!103 = !DIDerivedType(tag: DW_TAG_member, name: "sort_index", scope: !94, file: !22, line: 154, baseType: !9, size: 64, offset: 448)
!104 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !50, size: 64)
!105 = !{!106, !107, !108, !109, !110, !111, !112, !113, !114, !115, !116, !117, !46, !118, !119, !133, !134, !135, !136, !137, !138, !139, !140, !141, !142, !143, !144, !145, !146, !147, !148, !149, !150, !151, !152, !153, !154, !155, !156, !157, !158, !159, !160, !161, !162, !163, !164, !165, !166, !167, !168, !169, !170, !171, !172, !173, !174, !175, !176, !177, !178, !179, !180, !181, !182, !183, !184, !185, !186, !187, !188, !189, !190, !191, !192, !193, !194, !195, !196, !197, !198, !199, !200, !201, !202, !203, !204, !205, !206, !207, !208, !209, !210, !211, !212, !213, !214, !215, !216, !217, !218, !219, !220, !221, !222, !223, !224, !225, !226, !227, !228, !229, !230, !231, !232, !233, !234, !235, !236, !237, !238, !239, !240, !241, !242, !243, !244, !245, !246, !247, !248, !249, !250, !251, !252, !253, !254, !255, !256, !257, !258, !259, !260, !261, !262, !263, !264, !265, !266, !267, !268, !269, !270, !271, !272}
!106 = !DILocalVariable(name: "A", arg: 1, scope: !47, file: !1, line: 23, type: !50)
!107 = !DILocalVariable(name: "CF_marker", arg: 2, scope: !47, file: !1, line: 24, type: !9)
!108 = !DILocalVariable(name: "S", arg: 3, scope: !47, file: !1, line: 25, type: !50)
!109 = !DILocalVariable(name: "num_cpts_global", arg: 4, scope: !47, file: !1, line: 26, type: !11)
!110 = !DILocalVariable(name: "num_functions", arg: 5, scope: !47, file: !1, line: 27, type: !10)
!111 = !DILocalVariable(name: "dof_func", arg: 6, scope: !47, file: !1, line: 28, type: !9)
!112 = !DILocalVariable(name: "debug_flag", arg: 7, scope: !47, file: !1, line: 29, type: !10)
!113 = !DILocalVariable(name: "trunc_factor", arg: 8, scope: !47, file: !1, line: 30, type: !40)
!114 = !DILocalVariable(name: "P_max_elmts", arg: 9, scope: !47, file: !1, line: 31, type: !10)
!115 = !DILocalVariable(name: "weight_option", arg: 10, scope: !47, file: !1, line: 32, type: !10)
!116 = !DILocalVariable(name: "col_offd_S_to_A", arg: 11, scope: !47, file: !1, line: 33, type: !9)
!117 = !DILocalVariable(name: "P_ptr", arg: 12, scope: !47, file: !1, line: 34, type: !104)
!118 = !DILocalVariable(name: "comm_pkg", scope: !47, file: !1, line: 37, type: !20)
!119 = !DILocalVariable(name: "comm_handle", scope: !47, file: !1, line: 38, type: !120)
!120 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !121, size: 64)
!121 = !DIDerivedType(tag: DW_TAG_typedef, name: "hypre_ParCSRCommHandle", file: !22, line: 101, baseType: !122)
!122 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !22, line: 92, size: 320, elements: !123)
!123 = !{!124, !125, !126, !127, !128}
!124 = !DIDerivedType(tag: DW_TAG_member, name: "comm_pkg", scope: !122, file: !22, line: 94, baseType: !20, size: 64)
!125 = !DIDerivedType(tag: DW_TAG_member, name: "send_data", scope: !122, file: !22, line: 95, baseType: !7, size: 64, offset: 64)
!126 = !DIDerivedType(tag: DW_TAG_member, name: "recv_data", scope: !122, file: !22, line: 96, baseType: !7, size: 64, offset: 128)
!127 = !DIDerivedType(tag: DW_TAG_member, name: "num_requests", scope: !122, file: !22, line: 98, baseType: !10, size: 32, offset: 192)
!128 = !DIDerivedType(tag: DW_TAG_member, name: "requests", scope: !122, file: !22, line: 99, baseType: !129, size: 64, offset: 256)
!129 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !130, size: 64)
!130 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Request", file: !4, line: 430, baseType: !131)
!131 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !132, size: 64)
!132 = !DICompositeType(tag: DW_TAG_structure_type, name: "ompi_request_t", file: !4, line: 430, flags: DIFlagFwdDecl)
!133 = !DILocalVariable(name: "tmp_comm_pkg", scope: !47, file: !1, line: 39, type: !20)
!134 = !DILocalVariable(name: "A_diag", scope: !47, file: !1, line: 41, type: !62)
!135 = !DILocalVariable(name: "A_diag_data", scope: !47, file: !1, line: 42, type: !39)
!136 = !DILocalVariable(name: "A_diag_i", scope: !47, file: !1, line: 43, type: !9)
!137 = !DILocalVariable(name: "A_diag_j", scope: !47, file: !1, line: 44, type: !9)
!138 = !DILocalVariable(name: "A_offd", scope: !47, file: !1, line: 46, type: !62)
!139 = !DILocalVariable(name: "A_offd_data", scope: !47, file: !1, line: 47, type: !39)
!140 = !DILocalVariable(name: "A_offd_i", scope: !47, file: !1, line: 48, type: !9)
!141 = !DILocalVariable(name: "A_offd_j", scope: !47, file: !1, line: 49, type: !9)
!142 = !DILocalVariable(name: "col_map_offd_A", scope: !47, file: !1, line: 50, type: !11)
!143 = !DILocalVariable(name: "num_cols_offd_A", scope: !47, file: !1, line: 51, type: !10)
!144 = !DILocalVariable(name: "S_diag", scope: !47, file: !1, line: 53, type: !62)
!145 = !DILocalVariable(name: "S_diag_i", scope: !47, file: !1, line: 54, type: !9)
!146 = !DILocalVariable(name: "S_diag_j", scope: !47, file: !1, line: 55, type: !9)
!147 = !DILocalVariable(name: "S_offd", scope: !47, file: !1, line: 57, type: !62)
!148 = !DILocalVariable(name: "S_offd_i", scope: !47, file: !1, line: 58, type: !9)
!149 = !DILocalVariable(name: "S_offd_j", scope: !47, file: !1, line: 59, type: !9)
!150 = !DILocalVariable(name: "col_map_offd_S", scope: !47, file: !1, line: 60, type: !11)
!151 = !DILocalVariable(name: "num_cols_offd_S", scope: !47, file: !1, line: 61, type: !10)
!152 = !DILocalVariable(name: "col_map_offd", scope: !47, file: !1, line: 62, type: !11)
!153 = !DILocalVariable(name: "num_cols_offd", scope: !47, file: !1, line: 63, type: !10)
!154 = !DILocalVariable(name: "P", scope: !47, file: !1, line: 65, type: !50)
!155 = !DILocalVariable(name: "P_diag", scope: !47, file: !1, line: 66, type: !62)
!156 = !DILocalVariable(name: "P_diag_data", scope: !47, file: !1, line: 67, type: !39)
!157 = !DILocalVariable(name: "P_diag_i", scope: !47, file: !1, line: 68, type: !9)
!158 = !DILocalVariable(name: "P_diag_j", scope: !47, file: !1, line: 70, type: !9)
!159 = !DILocalVariable(name: "P_offd", scope: !47, file: !1, line: 72, type: !62)
!160 = !DILocalVariable(name: "P_offd_data", scope: !47, file: !1, line: 73, type: !39)
!161 = !DILocalVariable(name: "P_offd_i", scope: !47, file: !1, line: 74, type: !9)
!162 = !DILocalVariable(name: "P_offd_j", scope: !47, file: !1, line: 76, type: !9)
!163 = !DILocalVariable(name: "num_sends", scope: !47, file: !1, line: 78, type: !10)
!164 = !DILocalVariable(name: "int_buf_data", scope: !47, file: !1, line: 79, type: !9)
!165 = !DILocalVariable(name: "big_buf_data", scope: !47, file: !1, line: 80, type: !11)
!166 = !DILocalVariable(name: "send_map_start", scope: !47, file: !1, line: 81, type: !9)
!167 = !DILocalVariable(name: "send_map_elmt", scope: !47, file: !1, line: 82, type: !9)
!168 = !DILocalVariable(name: "send_procs", scope: !47, file: !1, line: 83, type: !9)
!169 = !DILocalVariable(name: "num_recvs", scope: !47, file: !1, line: 84, type: !10)
!170 = !DILocalVariable(name: "recv_vec_start", scope: !47, file: !1, line: 85, type: !9)
!171 = !DILocalVariable(name: "recv_procs", scope: !47, file: !1, line: 86, type: !9)
!172 = !DILocalVariable(name: "new_recv_vec_start", scope: !47, file: !1, line: 87, type: !9)
!173 = !DILocalVariable(name: "Pext_send_map_start", scope: !47, file: !1, line: 88, type: !18)
!174 = !DILocalVariable(name: "Pext_recv_vec_start", scope: !47, file: !1, line: 89, type: !18)
!175 = !DILocalVariable(name: "Pext_start", scope: !47, file: !1, line: 90, type: !9)
!176 = !DILocalVariable(name: "P_ncols", scope: !47, file: !1, line: 91, type: !9)
!177 = !DILocalVariable(name: "CF_marker_offd", scope: !47, file: !1, line: 93, type: !9)
!178 = !DILocalVariable(name: "dof_func_offd", scope: !47, file: !1, line: 94, type: !9)
!179 = !DILocalVariable(name: "P_marker", scope: !47, file: !1, line: 95, type: !9)
!180 = !DILocalVariable(name: "P_marker_offd", scope: !47, file: !1, line: 96, type: !9)
!181 = !DILocalVariable(name: "C_array", scope: !47, file: !1, line: 97, type: !9)
!182 = !DILocalVariable(name: "C_array_offd", scope: !47, file: !1, line: 98, type: !9)
!183 = !DILocalVariable(name: "pass_array", scope: !47, file: !1, line: 99, type: !9)
!184 = !DILocalVariable(name: "pass_pointer", scope: !47, file: !1, line: 100, type: !9)
!185 = !DILocalVariable(name: "P_diag_start", scope: !47, file: !1, line: 102, type: !9)
!186 = !DILocalVariable(name: "P_offd_start", scope: !47, file: !1, line: 103, type: !9)
!187 = !DILocalVariable(name: "P_diag_pass", scope: !47, file: !1, line: 104, type: !18)
!188 = !DILocalVariable(name: "P_offd_pass", scope: !47, file: !1, line: 105, type: !18)
!189 = !DILocalVariable(name: "Pext_pass", scope: !47, file: !1, line: 106, type: !18)
!190 = !DILocalVariable(name: "big_temp_pass", scope: !47, file: !1, line: 107, type: !11)
!191 = !DILocalVariable(name: "new_elmts", scope: !47, file: !1, line: 108, type: !19)
!192 = !DILocalVariable(name: "new_counter", scope: !47, file: !1, line: 109, type: !9)
!193 = !DILocalVariable(name: "loc", scope: !47, file: !1, line: 111, type: !9)
!194 = !DILocalVariable(name: "Pext_i", scope: !47, file: !1, line: 113, type: !9)
!195 = !DILocalVariable(name: "Pext_send_buffer", scope: !47, file: !1, line: 115, type: !11)
!196 = !DILocalVariable(name: "map_S_to_new", scope: !47, file: !1, line: 118, type: !9)
!197 = !DILocalVariable(name: "map_A_to_S", scope: !47, file: !1, line: 120, type: !9)
!198 = !DILocalVariable(name: "new_col_map_offd", scope: !47, file: !1, line: 121, type: !11)
!199 = !DILocalVariable(name: "col_map_offd_P", scope: !47, file: !1, line: 122, type: !11)
!200 = !DILocalVariable(name: "big_permute", scope: !47, file: !1, line: 123, type: !11)
!201 = !DILocalVariable(name: "permute", scope: !47, file: !1, line: 124, type: !9)
!202 = !DILocalVariable(name: "cnt", scope: !47, file: !1, line: 126, type: !10)
!203 = !DILocalVariable(name: "cnt_nz", scope: !47, file: !1, line: 127, type: !10)
!204 = !DILocalVariable(name: "total_nz", scope: !47, file: !1, line: 128, type: !10)
!205 = !DILocalVariable(name: "pass", scope: !47, file: !1, line: 129, type: !10)
!206 = !DILocalVariable(name: "num_passes", scope: !47, file: !1, line: 130, type: !10)
!207 = !DILocalVariable(name: "max_num_passes", scope: !47, file: !1, line: 131, type: !10)
!208 = !DILocalVariable(name: "n_fine", scope: !47, file: !1, line: 133, type: !10)
!209 = !DILocalVariable(name: "n_coarse", scope: !47, file: !1, line: 134, type: !10)
!210 = !DILocalVariable(name: "n_coarse_offd", scope: !47, file: !1, line: 135, type: !10)
!211 = !DILocalVariable(name: "n_SF", scope: !47, file: !1, line: 136, type: !10)
!212 = !DILocalVariable(name: "n_SF_offd", scope: !47, file: !1, line: 137, type: !10)
!213 = !DILocalVariable(name: "fine_to_coarse", scope: !47, file: !1, line: 139, type: !9)
!214 = !DILocalVariable(name: "fine_to_coarse_offd", scope: !47, file: !1, line: 140, type: !11)
!215 = !DILocalVariable(name: "assigned", scope: !47, file: !1, line: 142, type: !9)
!216 = !DILocalVariable(name: "assigned_offd", scope: !47, file: !1, line: 143, type: !9)
!217 = !DILocalVariable(name: "Pext_send_data", scope: !47, file: !1, line: 145, type: !39)
!218 = !DILocalVariable(name: "Pext_data", scope: !47, file: !1, line: 146, type: !39)
!219 = !DILocalVariable(name: "sum_C", scope: !47, file: !1, line: 148, type: !40)
!220 = !DILocalVariable(name: "sum_N", scope: !47, file: !1, line: 148, type: !40)
!221 = !DILocalVariable(name: "sum_C_pos", scope: !47, file: !1, line: 149, type: !40)
!222 = !DILocalVariable(name: "sum_C_neg", scope: !47, file: !1, line: 149, type: !40)
!223 = !DILocalVariable(name: "sum_N_pos", scope: !47, file: !1, line: 150, type: !40)
!224 = !DILocalVariable(name: "sum_N_neg", scope: !47, file: !1, line: 150, type: !40)
!225 = !DILocalVariable(name: "diagonal", scope: !47, file: !1, line: 151, type: !40)
!226 = !DILocalVariable(name: "alfa", scope: !47, file: !1, line: 152, type: !40)
!227 = !DILocalVariable(name: "beta", scope: !47, file: !1, line: 153, type: !40)
!228 = !DILocalVariable(name: "j_start", scope: !47, file: !1, line: 154, type: !10)
!229 = !DILocalVariable(name: "j_end", scope: !47, file: !1, line: 155, type: !10)
!230 = !DILocalVariable(name: "i", scope: !47, file: !1, line: 157, type: !10)
!231 = !DILocalVariable(name: "i1", scope: !47, file: !1, line: 157, type: !10)
!232 = !DILocalVariable(name: "j", scope: !47, file: !1, line: 158, type: !10)
!233 = !DILocalVariable(name: "j1", scope: !47, file: !1, line: 158, type: !10)
!234 = !DILocalVariable(name: "k", scope: !47, file: !1, line: 159, type: !10)
!235 = !DILocalVariable(name: "k1", scope: !47, file: !1, line: 159, type: !10)
!236 = !DILocalVariable(name: "k2", scope: !47, file: !1, line: 159, type: !10)
!237 = !DILocalVariable(name: "k3", scope: !47, file: !1, line: 159, type: !10)
!238 = !DILocalVariable(name: "pass_array_size", scope: !47, file: !1, line: 160, type: !10)
!239 = !DILocalVariable(name: "global_pass_array_size", scope: !47, file: !1, line: 161, type: !12)
!240 = !DILocalVariable(name: "local_pass_array_size", scope: !47, file: !1, line: 162, type: !12)
!241 = !DILocalVariable(name: "my_id", scope: !47, file: !1, line: 163, type: !10)
!242 = !DILocalVariable(name: "num_procs", scope: !47, file: !1, line: 163, type: !10)
!243 = !DILocalVariable(name: "index", scope: !47, file: !1, line: 164, type: !10)
!244 = !DILocalVariable(name: "start", scope: !47, file: !1, line: 164, type: !10)
!245 = !DILocalVariable(name: "my_first_cpt", scope: !47, file: !1, line: 165, type: !12)
!246 = !DILocalVariable(name: "total_global_cpts", scope: !47, file: !1, line: 166, type: !12)
!247 = !DILocalVariable(name: "big_value", scope: !47, file: !1, line: 167, type: !12)
!248 = !DILocalVariable(name: "p_cnt", scope: !47, file: !1, line: 168, type: !10)
!249 = !DILocalVariable(name: "total_nz_offd", scope: !47, file: !1, line: 169, type: !10)
!250 = !DILocalVariable(name: "cnt_nz_offd", scope: !47, file: !1, line: 170, type: !10)
!251 = !DILocalVariable(name: "cnt_offd", scope: !47, file: !1, line: 171, type: !10)
!252 = !DILocalVariable(name: "cnt_new", scope: !47, file: !1, line: 171, type: !10)
!253 = !DILocalVariable(name: "no_break", scope: !47, file: !1, line: 172, type: !10)
!254 = !DILocalVariable(name: "not_found", scope: !47, file: !1, line: 173, type: !10)
!255 = !DILocalVariable(name: "Pext_send_size", scope: !47, file: !1, line: 174, type: !10)
!256 = !DILocalVariable(name: "Pext_recv_size", scope: !47, file: !1, line: 175, type: !10)
!257 = !DILocalVariable(name: "old_Pext_send_size", scope: !47, file: !1, line: 176, type: !10)
!258 = !DILocalVariable(name: "old_Pext_recv_size", scope: !47, file: !1, line: 177, type: !10)
!259 = !DILocalVariable(name: "P_offd_size", scope: !47, file: !1, line: 178, type: !10)
!260 = !DILocalVariable(name: "local_index", scope: !47, file: !1, line: 179, type: !10)
!261 = !DILocalVariable(name: "new_num_cols_offd", scope: !47, file: !1, line: 180, type: !10)
!262 = !DILocalVariable(name: "num_cols_offd_P", scope: !47, file: !1, line: 181, type: !10)
!263 = !DILocalVariable(name: "num_threads", scope: !47, file: !1, line: 182, type: !10)
!264 = !DILocalVariable(name: "ns", scope: !47, file: !1, line: 183, type: !10)
!265 = !DILocalVariable(name: "ne", scope: !47, file: !1, line: 183, type: !10)
!266 = !DILocalVariable(name: "size", scope: !47, file: !1, line: 183, type: !10)
!267 = !DILocalVariable(name: "rest", scope: !47, file: !1, line: 183, type: !10)
!268 = !DILocalVariable(name: "pass_length", scope: !47, file: !1, line: 183, type: !10)
!269 = !DILocalVariable(name: "tmp_marker", scope: !47, file: !1, line: 184, type: !9)
!270 = !DILocalVariable(name: "tmp_marker_offd", scope: !47, file: !1, line: 184, type: !9)
!271 = !DILocalVariable(name: "tmp_array", scope: !47, file: !1, line: 185, type: !9)
!272 = !DILocalVariable(name: "tmp_array_offd", scope: !47, file: !1, line: 185, type: !9)
!273 = !DILocation(line: 0, scope: !47)
!274 = !DILocation(line: 41, column: 30, scope: !47)
!275 = !DILocation(line: 58, column: 21, scope: !47)
!276 = !DILocation(line: 59, column: 21, scope: !47)
!277 = !DILocation(line: 63, column: 4, scope: !47)
