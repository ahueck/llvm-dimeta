; RUN: %apply-verifier %s |& %filecheck %s
; REQUIRES: llvm-18 || llvm-19

; CHECK-COUNT-7: Location: "{{.*}}quark_stuff.c":"eo_fermion_force_3f":1293

source_filename = "quark_stuff.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.half_wilson_vector = type { [2 x %struct.su3_vector] }
%struct.su3_vector = type { [3 x %struct.complex] }
%struct.complex = type { float, float }
%struct.site = type { i16, i16, i16, i16, i8, i32, %struct.double_prn, i32, [4 x %struct.su3_matrix], [4 x %struct.anti_hermitmat], [4 x float], %struct.su3_vector, %struct.su3_vector, %struct.su3_vector, %struct.su3_vector, %struct.su3_vector, %struct.su3_vector, [4 x %struct.su3_vector], [4 x %struct.su3_vector], %struct.su3_vector, %struct.su3_matrix, %struct.su3_matrix }
%struct.double_prn = type { i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, float }
%struct.anti_hermitmat = type { %struct.complex, %struct.complex, %struct.complex, float, float, float, float }
%struct.su3_matrix = type { [3 x [3 x %struct.complex]] }

@act_path_coeff = external hidden unnamed_addr global [6 x float], align 16, !dbg !0
@sites_on_node = external local_unnamed_addr global i32, align 4
@lattice = external local_unnamed_addr global ptr, align 8

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #0

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #0

; Function Attrs: mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite)
declare !dbg !56 noalias noundef ptr @malloc(i64 noundef) local_unnamed_addr #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #2

; Function Attrs: mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite)
declare !dbg !59 void @free(ptr allocptr nocapture noundef) local_unnamed_addr #3

declare !dbg !62 void @scalar_mult_add_su3_vector(ptr noundef, ptr noundef, float noundef, ptr noundef) local_unnamed_addr #4

; Function Attrs: nounwind uwtable
define dso_local void @eo_fermion_force_3f(float noundef %eps, i32 noundef %nflav1, i32 noundef %x1_off, i32 noundef %nflav2, i32 noundef %x2_off) local_unnamed_addr #5 !dbg !65 {
entry:
  %m_coeff.i592 = alloca [2 x float], align 8, !DIAssignID !160
  %m_coeff.i576 = alloca [2 x float], align 8, !DIAssignID !161
  %m_coeff.i560 = alloca [2 x float], align 8, !DIAssignID !162
  %m_coeff.i = alloca [2 x float], align 8, !DIAssignID !163
  %DirectLinks = alloca [8 x i32], align 16, !DIAssignID !164
  call void @llvm.dbg.assign(metadata i1 undef, metadata !139, metadata !DIExpression(), metadata !164, metadata ptr %DirectLinks, metadata !DIExpression()), !dbg !165
  %OneLink = alloca [2 x float], align 8, !DIAssignID !166
  call void @llvm.dbg.assign(metadata i1 undef, metadata !146, metadata !DIExpression(), metadata !166, metadata ptr %OneLink, metadata !DIExpression()), !dbg !165
  %Lepage = alloca [2 x float], align 8, !DIAssignID !167
  call void @llvm.dbg.assign(metadata i1 undef, metadata !147, metadata !DIExpression(), metadata !167, metadata ptr %Lepage, metadata !DIExpression()), !dbg !165
  %Naik = alloca [2 x float], align 8, !DIAssignID !168
  call void @llvm.dbg.assign(metadata i1 undef, metadata !148, metadata !DIExpression(), metadata !168, metadata ptr %Naik, metadata !DIExpression()), !dbg !165
  %FiveSt = alloca [2 x float], align 8, !DIAssignID !169
  call void @llvm.dbg.assign(metadata i1 undef, metadata !149, metadata !DIExpression(), metadata !169, metadata ptr %FiveSt, metadata !DIExpression()), !dbg !165
  %ThreeSt = alloca [2 x float], align 8, !DIAssignID !170
  call void @llvm.dbg.assign(metadata i1 undef, metadata !150, metadata !DIExpression(), metadata !170, metadata ptr %ThreeSt, metadata !DIExpression()), !dbg !165
  %SevenSt = alloca [2 x float], align 8, !DIAssignID !171
  call void @llvm.dbg.assign(metadata i1 undef, metadata !151, metadata !DIExpression(), metadata !171, metadata ptr %SevenSt, metadata !DIExpression()), !dbg !165
  %mNaik = alloca [2 x float], align 8, !DIAssignID !172
  call void @llvm.dbg.assign(metadata i1 undef, metadata !152, metadata !DIExpression(), metadata !172, metadata ptr %mNaik, metadata !DIExpression()), !dbg !165
  %mLepage = alloca [2 x float], align 8, !DIAssignID !173
  call void @llvm.dbg.assign(metadata i1 undef, metadata !153, metadata !DIExpression(), metadata !173, metadata ptr %mLepage, metadata !DIExpression()), !dbg !165
  %mFiveSt = alloca [2 x float], align 8, !DIAssignID !174
  call void @llvm.dbg.assign(metadata i1 undef, metadata !154, metadata !DIExpression(), metadata !174, metadata ptr %mFiveSt, metadata !DIExpression()), !dbg !165
  %mThreeSt = alloca [2 x float], align 8, !DIAssignID !175
  call void @llvm.dbg.assign(metadata i1 undef, metadata !155, metadata !DIExpression(), metadata !175, metadata ptr %mThreeSt, metadata !DIExpression()), !dbg !165
  %mSevenSt = alloca [2 x float], align 8, !DIAssignID !176
  call void @llvm.dbg.assign(metadata i1 undef, metadata !156, metadata !DIExpression(), metadata !176, metadata ptr %mSevenSt, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata float %eps, metadata !68, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata i32 %nflav1, metadata !69, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata i32 %x1_off, metadata !70, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata i32 %nflav2, metadata !71, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata i32 %x2_off, metadata !72, metadata !DIExpression()), !dbg !165
  call void @llvm.lifetime.start.p0(i64 32, ptr nonnull %DirectLinks) #8, !dbg !177
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %OneLink) #8, !dbg !178
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %Lepage) #8, !dbg !178
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %Naik) #8, !dbg !178
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %FiveSt) #8, !dbg !178
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %ThreeSt) #8, !dbg !178
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %SevenSt) #8, !dbg !178
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %mNaik) #8, !dbg !179
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %mLepage) #8, !dbg !179
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %mFiveSt) #8, !dbg !179
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %mThreeSt) #8, !dbg !179
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %mSevenSt) #8, !dbg !179
  %0 = insertelement <2 x i32> poison, i32 %nflav1, i64 0, !dbg !180
  %1 = insertelement <2 x i32> %0, i32 %nflav2, i64 1, !dbg !180
  %2 = sitofp <2 x i32> %1 to <2 x double>, !dbg !180
  %conv1 = fpext float %eps to double, !dbg !181
  tail call void @llvm.dbg.value(metadata float undef, metadata !145, metadata !DIExpression()), !dbg !165
  %3 = load <4 x float>, ptr @act_path_coeff, align 16, !dbg !182
  call void @llvm.dbg.assign(metadata float undef, metadata !146, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !183, metadata ptr %OneLink, metadata !DIExpression()), !dbg !165
  %4 = load <4 x float>, ptr getelementptr inbounds ([6 x float], ptr @act_path_coeff, i64 0, i64 1), align 4, !dbg !184
  call void @llvm.dbg.assign(metadata float undef, metadata !148, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !185, metadata ptr %Naik, metadata !DIExpression()), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !152, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !186, metadata ptr %mNaik, metadata !DIExpression()), !dbg !165
  %5 = load <4 x float>, ptr getelementptr inbounds ([6 x float], ptr @act_path_coeff, i64 0, i64 2), align 8, !dbg !187
  call void @llvm.dbg.assign(metadata float undef, metadata !150, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !188, metadata ptr %ThreeSt, metadata !DIExpression()), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !155, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !189, metadata ptr %mThreeSt, metadata !DIExpression()), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !149, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !190, metadata ptr %FiveSt, metadata !DIExpression()), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !154, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !191, metadata ptr %mFiveSt, metadata !DIExpression()), !dbg !165
  %6 = load float, ptr getelementptr inbounds ([6 x float], ptr @act_path_coeff, i64 0, i64 4), align 16, !dbg !192, !tbaa !193
  call void @llvm.dbg.assign(metadata float undef, metadata !151, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !197, metadata ptr %SevenSt, metadata !DIExpression()), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !156, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !198, metadata ptr %mSevenSt, metadata !DIExpression()), !dbg !165
  %7 = load float, ptr getelementptr inbounds ([6 x float], ptr @act_path_coeff, i64 0, i64 5), align 4, !dbg !199, !tbaa !193
  call void @llvm.dbg.assign(metadata float undef, metadata !147, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !200, metadata ptr %Lepage, metadata !DIExpression()), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !153, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !201, metadata ptr %mLepage, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata float undef, metadata !145, metadata !DIExpression()), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !146, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !183, metadata ptr %OneLink, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !148, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !185, metadata ptr %Naik, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !152, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !186, metadata ptr %mNaik, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !150, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !188, metadata ptr %ThreeSt, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !155, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !189, metadata ptr %mThreeSt, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !165
  %arrayidx48 = getelementptr inbounds [2 x float], ptr %FiveSt, i64 0, i64 1, !dbg !202
  call void @llvm.dbg.assign(metadata float undef, metadata !149, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !190, metadata ptr %arrayidx48, metadata !DIExpression()), !dbg !165
  call void @llvm.dbg.assign(metadata float undef, metadata !154, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !191, metadata ptr %mFiveSt, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !165
  %8 = fmul <2 x double> %2, <double 2.500000e-01, double 2.500000e-01>, !dbg !203
  %9 = fmul <2 x double> %8, <double 2.000000e+00, double 2.000000e+00>, !dbg !204
  %10 = insertelement <2 x double> poison, double %conv1, i64 0, !dbg !205
  %11 = shufflevector <2 x double> %10, <2 x double> poison, <2 x i32> zeroinitializer, !dbg !205
  %12 = fmul <2 x double> %9, %11, !dbg !205
  %13 = fptrunc <2 x double> %12 to <2 x float>, !dbg !206
  %14 = insertelement <2 x float> poison, float %6, i64 0, !dbg !207
  %15 = shufflevector <2 x float> %14, <2 x float> poison, <2 x i32> zeroinitializer, !dbg !207
  %16 = fmul <2 x float> %15, %13, !dbg !207
  %17 = shufflevector <4 x float> %3, <4 x float> poison, <2 x i32> zeroinitializer, !dbg !208
  %18 = fmul <2 x float> %17, %13, !dbg !208
  store <2 x float> %18, ptr %OneLink, align 8, !dbg !209, !tbaa !193, !DIAssignID !183
  %19 = shufflevector <4 x float> %4, <4 x float> poison, <2 x i32> zeroinitializer, !dbg !210
  %20 = fmul <2 x float> %19, %13, !dbg !210
  store <2 x float> %20, ptr %Naik, align 8, !dbg !211, !tbaa !193, !DIAssignID !185
  %21 = fneg <2 x float> %20, !dbg !212
  store <2 x float> %21, ptr %mNaik, align 8, !dbg !213, !tbaa !193, !DIAssignID !186
  %22 = shufflevector <4 x float> %5, <4 x float> poison, <2 x i32> zeroinitializer, !dbg !214
  %23 = fmul <2 x float> %22, %13, !dbg !214
  store <2 x float> %23, ptr %ThreeSt, align 8, !dbg !215, !tbaa !193, !DIAssignID !188
  %24 = fneg <2 x float> %23, !dbg !216
  store <2 x float> %24, ptr %mThreeSt, align 8, !dbg !217, !tbaa !193, !DIAssignID !189
  %25 = shufflevector <4 x float> %3, <4 x float> poison, <2 x i32> <i32 3, i32 3>, !dbg !218
  %26 = fmul <2 x float> %25, %13, !dbg !218
  store <2 x float> %26, ptr %FiveSt, align 8, !dbg !219, !tbaa !193, !DIAssignID !190
  %27 = fneg <2 x float> %26, !dbg !220
  store <2 x float> %27, ptr %mFiveSt, align 8, !dbg !221, !tbaa !193, !DIAssignID !191
  store <2 x float> %16, ptr %SevenSt, align 8, !dbg !222, !tbaa !193, !DIAssignID !197
  call void @llvm.dbg.assign(metadata float undef, metadata !151, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !197, metadata ptr %SevenSt, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !165
  %28 = fneg <2 x float> %16, !dbg !223
  store <2 x float> %28, ptr %mSevenSt, align 8, !dbg !224, !tbaa !193, !DIAssignID !198
  call void @llvm.dbg.assign(metadata float undef, metadata !156, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !198, metadata ptr %mSevenSt, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !165
  %arrayidx58 = getelementptr inbounds [2 x float], ptr %Lepage, i64 0, i64 1, !dbg !225
  %29 = insertelement <2 x float> poison, float %7, i64 0, !dbg !226
  %30 = shufflevector <2 x float> %29, <2 x float> poison, <2 x i32> zeroinitializer, !dbg !226
  %31 = fmul <2 x float> %30, %13, !dbg !226
  store <2 x float> %31, ptr %Lepage, align 8, !dbg !227, !tbaa !193, !DIAssignID !200
  call void @llvm.dbg.assign(metadata float undef, metadata !147, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !200, metadata ptr %arrayidx58, metadata !DIExpression()), !dbg !165
  %32 = fneg <2 x float> %31, !dbg !228
  store <2 x float> %32, ptr %mLepage, align 8, !dbg !229, !tbaa !193, !DIAssignID !201
  call void @llvm.dbg.assign(metadata float undef, metadata !153, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !201, metadata ptr %mLepage, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !165
  tail call void @llvm.dbg.value(metadata i32 0, metadata !135, metadata !DIExpression()), !dbg !165
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 16 dereferenceable(32) %DirectLinks, i8 0, i64 32, i1 false), !dbg !230, !tbaa !233
  tail call void @llvm.dbg.value(metadata i64 poison, metadata !135, metadata !DIExpression()), !dbg !165
  %33 = load i32, ptr @sites_on_node, align 4, !tbaa !233
  %conv68 = sext i32 %33 to i64
  %mul69 = mul nsw i64 %conv68, 48
  tail call void @llvm.dbg.value(metadata i32 0, metadata !135, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata i64 0, metadata !135, metadata !DIExpression()), !dbg !165
  %call = tail call noalias ptr @malloc(i64 noundef %mul69) #9, !dbg !235
  tail call void @llvm.dbg.value(metadata ptr %call, metadata !157, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 64)), !dbg !165
  tail call void @llvm.dbg.value(metadata i64 1, metadata !135, metadata !DIExpression()), !dbg !165
  %call.1 = tail call noalias ptr @malloc(i64 noundef %mul69) #9, !dbg !235
  tail call void @llvm.dbg.value(metadata ptr %call.1, metadata !157, metadata !DIExpression(DW_OP_LLVM_fragment, 64, 64)), !dbg !165
  tail call void @llvm.dbg.value(metadata i64 2, metadata !135, metadata !DIExpression()), !dbg !165
  %call.2 = tail call noalias ptr @malloc(i64 noundef %mul69) #9, !dbg !235
  tail call void @llvm.dbg.value(metadata ptr %call.2, metadata !157, metadata !DIExpression(DW_OP_LLVM_fragment, 128, 64)), !dbg !165
  tail call void @llvm.dbg.value(metadata i64 3, metadata !135, metadata !DIExpression()), !dbg !165
  %call.3 = tail call noalias ptr @malloc(i64 noundef %mul69) #9, !dbg !235
  tail call void @llvm.dbg.value(metadata ptr %call.3, metadata !157, metadata !DIExpression(DW_OP_LLVM_fragment, 192, 64)), !dbg !165
  tail call void @llvm.dbg.value(metadata i64 4, metadata !135, metadata !DIExpression()), !dbg !165
  %call.4 = tail call noalias ptr @malloc(i64 noundef %mul69) #9, !dbg !235
  tail call void @llvm.dbg.value(metadata ptr %call.4, metadata !157, metadata !DIExpression(DW_OP_LLVM_fragment, 256, 64)), !dbg !165
  tail call void @llvm.dbg.value(metadata i64 5, metadata !135, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr poison, metadata !157, metadata !DIExpression(DW_OP_LLVM_fragment, 320, 64)), !dbg !165
  tail call void @llvm.dbg.value(metadata i64 6, metadata !135, metadata !DIExpression()), !dbg !165
  %call.6 = tail call noalias ptr @malloc(i64 noundef %mul69) #9, !dbg !235
  tail call void @llvm.dbg.value(metadata ptr %call.6, metadata !157, metadata !DIExpression(DW_OP_LLVM_fragment, 384, 64)), !dbg !165
  tail call void @llvm.dbg.value(metadata i64 7, metadata !135, metadata !DIExpression()), !dbg !165
  %call.7 = tail call noalias ptr @malloc(i64 noundef %mul69) #9, !dbg !235
  tail call void @llvm.dbg.value(metadata ptr %call.7, metadata !157, metadata !DIExpression(DW_OP_LLVM_fragment, 448, 64)), !dbg !165
  tail call void @llvm.dbg.value(metadata i64 8, metadata !135, metadata !DIExpression()), !dbg !165
  %call77 = tail call noalias ptr @malloc(i64 noundef %mul69) #9, !dbg !238
  tail call void @llvm.dbg.value(metadata ptr %call77, metadata !159, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr poison, metadata !74, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata i32 0, metadata !73, metadata !DIExpression()), !dbg !165
  %cmp79613 = icmp sgt i32 %33, 0, !dbg !239
  br i1 %cmp79613, label %for.body81.lr.ph, label %for.cond94.preheader, !dbg !242

for.body81.lr.ph:                                 ; preds = %entry
  %34 = load ptr, ptr @lattice, align 8, !dbg !242, !tbaa !243
  tail call void @llvm.dbg.value(metadata ptr %34, metadata !74, metadata !DIExpression()), !dbg !165
  %idx.ext = sext i32 %x1_off to i64
  %idx.ext89 = sext i32 %x2_off to i64
  %wide.trip.count = zext nneg i32 %33 to i64, !dbg !239
  %xtraiter = and i64 %wide.trip.count, 1, !dbg !242
  %35 = icmp eq i32 %33, 1, !dbg !242
  br i1 %35, label %for.cond94.preheader.loopexit.unr-lcssa, label %for.body81.lr.ph.new, !dbg !242

for.body81.lr.ph.new:                             ; preds = %for.body81.lr.ph
  %unroll_iter = and i64 %wide.trip.count, 2147483646, !dbg !242
  br label %for.body81, !dbg !242

for.cond94.preheader.loopexit.unr-lcssa:          ; preds = %for.body81, %for.body81.lr.ph
  %indvars.iv.unr = phi i64 [ 0, %for.body81.lr.ph ], [ %indvars.iv.next.1, %for.body81 ]
  %s.0614.unr = phi ptr [ %34, %for.body81.lr.ph ], [ %incdec.ptr.1, %for.body81 ]
  %lcmp.mod.not = icmp eq i64 %xtraiter, 0, !dbg !242
  br i1 %lcmp.mod.not, label %for.cond94.preheader, label %for.body81.epil, !dbg !242

for.body81.epil:                                  ; preds = %for.cond94.preheader.loopexit.unr-lcssa
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv.unr, metadata !73, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr %s.0614.unr, metadata !74, metadata !DIExpression()), !dbg !165
  %arrayidx83.epil = getelementptr inbounds %struct.half_wilson_vector, ptr %call77, i64 %indvars.iv.unr, !dbg !245
  %add.ptr.epil = getelementptr inbounds i8, ptr %s.0614.unr, i64 %idx.ext, !dbg !247
  tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(24) %arrayidx83.epil, ptr noundef nonnull align 4 dereferenceable(24) %add.ptr.epil, i64 24, i1 false), !dbg !248, !tbaa.struct !249
  %arrayidx88.epil = getelementptr inbounds [2 x %struct.su3_vector], ptr %arrayidx83.epil, i64 0, i64 1, !dbg !251
  %add.ptr90.epil = getelementptr inbounds i8, ptr %s.0614.unr, i64 %idx.ext89, !dbg !252
  tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(24) %arrayidx88.epil, ptr noundef nonnull align 4 dereferenceable(24) %add.ptr90.epil, i64 24, i1 false), !dbg !253, !tbaa.struct !249
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv.unr, metadata !73, metadata !DIExpression(DW_OP_plus_uconst, 1, DW_OP_stack_value)), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr %s.0614.unr, metadata !74, metadata !DIExpression(DW_OP_plus_uconst, 1080, DW_OP_stack_value)), !dbg !165
  br label %for.cond94.preheader, !dbg !254

for.cond94.preheader:                             ; preds = %for.body81.epil, %for.cond94.preheader.loopexit.unr-lcssa, %entry
  tail call void @llvm.dbg.value(metadata i32 0, metadata !138, metadata !DIExpression()), !dbg !165
  %36 = extractelement <2 x float> %26, i64 0, !dbg !254
  %37 = extractelement <2 x float> %26, i64 1, !dbg !254
  br label %for.cond98.preheader, !dbg !254

for.body81:                                       ; preds = %for.body81, %for.body81.lr.ph.new
  %indvars.iv = phi i64 [ 0, %for.body81.lr.ph.new ], [ %indvars.iv.next.1, %for.body81 ]
  %s.0614 = phi ptr [ %34, %for.body81.lr.ph.new ], [ %incdec.ptr.1, %for.body81 ]
  %niter = phi i64 [ 0, %for.body81.lr.ph.new ], [ %niter.next.1, %for.body81 ]
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv, metadata !73, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr %s.0614, metadata !74, metadata !DIExpression()), !dbg !165
  %arrayidx83 = getelementptr inbounds %struct.half_wilson_vector, ptr %call77, i64 %indvars.iv, !dbg !245
  %add.ptr = getelementptr inbounds i8, ptr %s.0614, i64 %idx.ext, !dbg !247
  tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(24) %arrayidx83, ptr noundef nonnull align 4 dereferenceable(24) %add.ptr, i64 24, i1 false), !dbg !248, !tbaa.struct !249
  %arrayidx88 = getelementptr inbounds [2 x %struct.su3_vector], ptr %arrayidx83, i64 0, i64 1, !dbg !251
  %add.ptr90 = getelementptr inbounds i8, ptr %s.0614, i64 %idx.ext89, !dbg !252
  tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(24) %arrayidx88, ptr noundef nonnull align 4 dereferenceable(24) %add.ptr90, i64 24, i1 false), !dbg !253, !tbaa.struct !249
  %indvars.iv.next = or disjoint i64 %indvars.iv, 1, !dbg !239
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv.next, metadata !73, metadata !DIExpression()), !dbg !165
  %incdec.ptr = getelementptr inbounds %struct.site, ptr %s.0614, i64 1, !dbg !239
  tail call void @llvm.dbg.value(metadata ptr %incdec.ptr, metadata !74, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv.next, metadata !73, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr %incdec.ptr, metadata !74, metadata !DIExpression()), !dbg !165
  %arrayidx83.1 = getelementptr inbounds %struct.half_wilson_vector, ptr %call77, i64 %indvars.iv.next, !dbg !245
  %add.ptr.1 = getelementptr inbounds i8, ptr %incdec.ptr, i64 %idx.ext, !dbg !247
  tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(24) %arrayidx83.1, ptr noundef nonnull align 4 dereferenceable(24) %add.ptr.1, i64 24, i1 false), !dbg !248, !tbaa.struct !249
  %arrayidx88.1 = getelementptr inbounds [2 x %struct.su3_vector], ptr %arrayidx83.1, i64 0, i64 1, !dbg !251
  %add.ptr90.1 = getelementptr inbounds i8, ptr %incdec.ptr, i64 %idx.ext89, !dbg !252
  tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(24) %arrayidx88.1, ptr noundef nonnull align 4 dereferenceable(24) %add.ptr90.1, i64 24, i1 false), !dbg !253, !tbaa.struct !249
  %indvars.iv.next.1 = add nuw nsw i64 %indvars.iv, 2, !dbg !239
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv.next.1, metadata !73, metadata !DIExpression()), !dbg !165
  %incdec.ptr.1 = getelementptr inbounds %struct.site, ptr %s.0614, i64 2, !dbg !239
  tail call void @llvm.dbg.value(metadata ptr %incdec.ptr.1, metadata !74, metadata !DIExpression()), !dbg !165
  %niter.next.1 = add i64 %niter, 2, !dbg !242
  %niter.ncmp.1 = icmp eq i64 %niter.next.1, %unroll_iter, !dbg !242
  br i1 %niter.ncmp.1, label %for.cond94.preheader.loopexit.unr-lcssa, label %for.body81, !dbg !242, !llvm.loop !256

for.cond98.preheader:                             ; preds = %for.inc409, %for.cond94.preheader
  %38 = phi float [ %37, %for.cond94.preheader ], [ %94, %for.inc409 ]
  %39 = phi float [ %36, %for.cond94.preheader ], [ %95, %for.inc409 ]
  %40 = phi float [ %37, %for.cond94.preheader ], [ %96, %for.inc409 ]
  %41 = phi float [ %36, %for.cond94.preheader ], [ %97, %for.inc409 ]
  %indvars.iv648 = phi i64 [ 0, %for.cond94.preheader ], [ %indvars.iv.next649, %for.inc409 ]
  %42 = phi <2 x float> [ %16, %for.cond94.preheader ], [ %98, %for.inc409 ]
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv648, metadata !138, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata i32 0, metadata !135, metadata !DIExpression()), !dbg !165
  %43 = trunc i64 %indvars.iv648 to i32
  %sub = xor i32 %43, 7
  %cmp110 = icmp ult i64 %indvars.iv648, 4
  %44 = zext i32 %sub to i64, !dbg !258
  br label %for.body101, !dbg !258

for.body101:                                      ; preds = %for.inc406, %for.cond98.preheader
  %45 = phi float [ %38, %for.cond98.preheader ], [ %94, %for.inc406 ]
  %46 = phi float [ %39, %for.cond98.preheader ], [ %95, %for.inc406 ]
  %47 = phi float [ %40, %for.cond98.preheader ], [ %96, %for.inc406 ]
  %48 = phi float [ %41, %for.cond98.preheader ], [ %97, %for.inc406 ]
  %indvars.iv644 = phi i64 [ 0, %for.cond98.preheader ], [ %indvars.iv.next645, %for.inc406 ]
  %49 = phi <2 x float> [ %42, %for.cond98.preheader ], [ %98, %for.inc406 ]
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv644, metadata !135, metadata !DIExpression()), !dbg !165
  %cmp102.not = icmp eq i64 %indvars.iv644, %indvars.iv648, !dbg !262
  %cmp104.not = icmp eq i64 %indvars.iv644, %44
  %or.cond629 = or i1 %cmp102.not, %cmp104.not, !dbg !265
  br i1 %or.cond629, label %for.inc406, label %if.then, !dbg !265

if.then:                                          ; preds = %for.body101
  %50 = trunc i64 %indvars.iv644 to i32, !dbg !266
  %sub107 = xor i32 %50, 7, !dbg !266
  tail call void @u_shift_hw_fermion(ptr noundef %call77, ptr noundef %call, i32 noundef %sub107), !dbg !268
  tail call void @u_shift_hw_fermion(ptr noundef %call, ptr noundef %call.7, i32 noundef %43), !dbg !269
  br i1 %cmp110, label %if.then112, label %if.end, !dbg !270

if.then112:                                       ; preds = %if.then
  call void @add_3f_force_to_mom(ptr noundef %call.7, ptr noundef %call, i32 noundef %43, ptr noundef nonnull %mThreeSt), !dbg !271
  br label %if.end, !dbg !274

if.end:                                           ; preds = %if.then112, %if.then
  tail call void @llvm.dbg.value(metadata i32 0, metadata !136, metadata !DIExpression()), !dbg !165
  %51 = load <2 x float>, ptr %ThreeSt, align 8
  %52 = extractelement <2 x float> %51, i64 0, !dbg !275
  %53 = extractelement <2 x float> %51, i64 1, !dbg !280
  br label %for.body118, !dbg !281

for.body118:                                      ; preds = %for.inc297, %if.end
  %54 = phi float [ %45, %if.end ], [ %79, %for.inc297 ]
  %55 = phi float [ %46, %if.end ], [ %80, %for.inc297 ]
  %56 = phi float [ %47, %if.end ], [ %81, %for.inc297 ]
  %57 = phi float [ %48, %if.end ], [ %82, %for.inc297 ]
  %nu.0622 = phi i32 [ 0, %if.end ], [ %inc298, %for.inc297 ]
  %58 = phi <2 x float> [ %49, %if.end ], [ %83, %for.inc297 ]
  tail call void @llvm.dbg.value(metadata i32 %nu.0622, metadata !136, metadata !DIExpression()), !dbg !165
  %59 = zext nneg i32 %nu.0622 to i64, !dbg !282
  %cmp119.not = icmp eq i64 %indvars.iv644, %59, !dbg !282
  %cmp123.not = icmp eq i32 %nu.0622, %sub107
  %or.cond = or i1 %cmp119.not, %cmp123.not, !dbg !283
  %cmp126.not = icmp eq i64 %indvars.iv648, %59
  %or.cond553 = or i1 %cmp126.not, %or.cond, !dbg !283
  %cmp130.not = icmp eq i32 %nu.0622, %sub
  %or.cond554 = or i1 %cmp130.not, %or.cond553, !dbg !283
  br i1 %or.cond554, label %for.inc297, label %if.then132, !dbg !283

if.then132:                                       ; preds = %for.body118
  %sub135 = xor i32 %nu.0622, 7, !dbg !284
  tail call void @u_shift_hw_fermion(ptr noundef %call, ptr noundef %call.1, i32 noundef %sub135), !dbg !285
  tail call void @u_shift_hw_fermion(ptr noundef %call.1, ptr noundef %call.6, i32 noundef %43), !dbg !286
  br i1 %cmp110, label %if.then140, label %if.end144, !dbg !287

if.then140:                                       ; preds = %if.then132
  call void @add_3f_force_to_mom(ptr noundef %call.6, ptr noundef %call.1, i32 noundef %43, ptr noundef nonnull %FiveSt), !dbg !288
  %.pre = load float, ptr %FiveSt, align 8
  %.pre669 = load float, ptr %arrayidx48, align 4
  br label %if.end144, !dbg !291

if.end144:                                        ; preds = %if.then140, %if.then132
  %60 = phi float [ %.pre669, %if.then140 ], [ %54, %if.then132 ]
  %61 = phi float [ %.pre, %if.then140 ], [ %55, %if.then132 ]
  %62 = phi float [ %.pre669, %if.then140 ], [ %56, %if.then132 ]
  %63 = phi float [ %.pre, %if.then140 ], [ %57, %if.then132 ]
  tail call void @llvm.dbg.value(metadata i32 0, metadata !137, metadata !DIExpression()), !dbg !165
  br label %for.body148, !dbg !292

for.body148:                                      ; preds = %for.inc238, %if.end144
  %rho.0618 = phi i32 [ 0, %if.end144 ], [ %inc239, %for.inc238 ]
  %64 = phi <2 x float> [ %58, %if.end144 ], [ %73, %for.inc238 ]
  tail call void @llvm.dbg.value(metadata i32 %rho.0618, metadata !137, metadata !DIExpression()), !dbg !165
  %65 = zext nneg i32 %rho.0618 to i64, !dbg !294
  %cmp149.not = icmp eq i64 %indvars.iv644, %65, !dbg !294
  %cmp153.not = icmp eq i32 %rho.0618, %sub107
  %or.cond555 = or i1 %cmp149.not, %cmp153.not, !dbg !297
  %cmp156.not = icmp eq i32 %rho.0618, %nu.0622
  %or.cond556 = or i1 %cmp156.not, %or.cond555, !dbg !297
  %cmp160.not = icmp eq i32 %rho.0618, %sub135
  %or.cond557 = or i1 %cmp160.not, %or.cond556, !dbg !297
  %cmp163.not = icmp eq i64 %indvars.iv648, %65
  %or.cond558 = or i1 %cmp163.not, %or.cond557, !dbg !297
  %cmp167.not = icmp eq i32 %rho.0618, %sub
  %or.cond559 = or i1 %cmp167.not, %or.cond558, !dbg !297
  br i1 %or.cond559, label %for.inc238, label %if.then169, !dbg !297

if.then169:                                       ; preds = %for.body148
  %sub172 = xor i32 %rho.0618, 7, !dbg !298
  tail call void @u_shift_hw_fermion(ptr noundef %call.1, ptr noundef %call.2, i32 noundef %sub172), !dbg !300
  tail call void @u_shift_hw_fermion(ptr noundef %call.2, ptr noundef %call.3, i32 noundef %43), !dbg !301
  br i1 %cmp110, label %if.then177, label %if.end181, !dbg !302

if.then177:                                       ; preds = %if.then169
  call void @add_3f_force_to_mom(ptr noundef %call.3, ptr noundef %call.2, i32 noundef %43, ptr noundef nonnull %mSevenSt), !dbg !303
  br label %if.end181, !dbg !306

if.end181:                                        ; preds = %if.then177, %if.then169
  tail call void @u_shift_hw_fermion(ptr noundef %call.3, ptr noundef %call.4, i32 noundef %rho.0618), !dbg !307
  call void @llvm.dbg.assign(metadata i1 undef, metadata !308, metadata !DIExpression(), metadata !163, metadata ptr %m_coeff.i, metadata !DIExpression()), !dbg !321
  call void @llvm.dbg.value(metadata i32 %rho.0618, metadata !314, metadata !DIExpression()), !dbg !321
  call void @llvm.dbg.value(metadata i64 %indvars.iv648, metadata !315, metadata !DIExpression()), !dbg !321
  call void @llvm.dbg.value(metadata ptr %SevenSt, metadata !316, metadata !DIExpression()), !dbg !321
  call void @llvm.dbg.value(metadata ptr %call.1, metadata !317, metadata !DIExpression()), !dbg !321
  call void @llvm.dbg.value(metadata ptr %call.3, metadata !318, metadata !DIExpression()), !dbg !321
  call void @llvm.dbg.value(metadata ptr %call.2, metadata !319, metadata !DIExpression()), !dbg !321
  call void @llvm.dbg.value(metadata ptr %call.4, metadata !320, metadata !DIExpression()), !dbg !321
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %m_coeff.i) #8, !dbg !323
  call void @llvm.dbg.assign(metadata float undef, metadata !308, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !324, metadata ptr %m_coeff.i, metadata !DIExpression()), !dbg !321
  %66 = fneg <2 x float> %64, !dbg !325
  store <2 x float> %66, ptr %m_coeff.i, align 8, !dbg !326, !tbaa !193, !DIAssignID !324
  call void @llvm.dbg.assign(metadata float undef, metadata !308, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !324, metadata ptr %m_coeff.i, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !321
  %cmp.i = icmp ult i32 %rho.0618, 4, !dbg !327
  br i1 %cmp.i, label %if.then.i, label %if.else7.i, !dbg !329

if.then.i:                                        ; preds = %if.end181
  br i1 %cmp110, label %if.then6.i, label %if.else.i, !dbg !330

if.then6.i:                                       ; preds = %if.then.i
  call void @add_3f_force_to_mom(ptr noundef %call.4, ptr noundef %call.1, i32 noundef %rho.0618, ptr noundef nonnull %SevenSt), !dbg !332
  br label %side_link_3f_force.exit, !dbg !332

if.else.i:                                        ; preds = %if.then.i
  call void @add_3f_force_to_mom(ptr noundef %call.1, ptr noundef %call.4, i32 noundef %sub172, ptr noundef nonnull %m_coeff.i), !dbg !334
  br label %side_link_3f_force.exit

if.else7.i:                                       ; preds = %if.end181
  br i1 %cmp110, label %if.then9.i, label %if.else11.i, !dbg !335

if.then9.i:                                       ; preds = %if.else7.i
  call void @add_3f_force_to_mom(ptr noundef %call.3, ptr noundef %call.2, i32 noundef %rho.0618, ptr noundef nonnull %m_coeff.i), !dbg !337
  br label %side_link_3f_force.exit, !dbg !337

if.else11.i:                                      ; preds = %if.else7.i
  call void @add_3f_force_to_mom(ptr noundef %call.2, ptr noundef %call.3, i32 noundef %sub172, ptr noundef nonnull %SevenSt), !dbg !339
  br label %side_link_3f_force.exit

side_link_3f_force.exit:                          ; preds = %if.else11.i, %if.then9.i, %if.else.i, %if.then6.i
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %m_coeff.i) #8, !dbg !340
  tail call void @llvm.dbg.value(metadata float %div191, metadata !143, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32)), !dbg !165
  %67 = load <2 x float>, ptr %SevenSt, align 8, !dbg !341, !tbaa !193
  %68 = extractelement <2 x float> %67, i64 0, !dbg !342
  %div191 = fdiv float %68, %63, !dbg !342
  %69 = extractelement <2 x float> %67, i64 1, !dbg !343
  %div195 = fdiv float %69, %62, !dbg !343
  tail call void @llvm.dbg.value(metadata float %div195, metadata !143, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32)), !dbg !165
  tail call void @llvm.dbg.value(metadata i32 0, metadata !73, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr poison, metadata !74, metadata !DIExpression()), !dbg !165
  %70 = load i32, ptr @sites_on_node, align 4, !dbg !344, !tbaa !233
  %cmp198616 = icmp sgt i32 %70, 0, !dbg !344
  br i1 %cmp198616, label %for.body200, label %for.inc238, !dbg !347

for.body200:                                      ; preds = %for.body200, %side_link_3f_force.exit
  %indvars.iv633 = phi i64 [ %indvars.iv.next634, %for.body200 ], [ 0, %side_link_3f_force.exit ]
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv633, metadata !73, metadata !DIExpression()), !dbg !165
  %arrayidx203 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.6, i64 %indvars.iv633, !dbg !348
  %arrayidx208 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.4, i64 %indvars.iv633, !dbg !350
  tail call void @scalar_mult_add_su3_vector(ptr noundef %arrayidx203, ptr noundef %arrayidx208, float noundef %div191, ptr noundef %arrayidx203) #8, !dbg !351
  %arrayidx221 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.6, i64 %indvars.iv633, i32 0, i64 1, !dbg !352
  %arrayidx226 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.4, i64 %indvars.iv633, i32 0, i64 1, !dbg !353
  tail call void @scalar_mult_add_su3_vector(ptr noundef nonnull %arrayidx221, ptr noundef nonnull %arrayidx226, float noundef %div195, ptr noundef nonnull %arrayidx221) #8, !dbg !354
  %indvars.iv.next634 = add nuw nsw i64 %indvars.iv633, 1, !dbg !344
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv.next634, metadata !73, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr poison, metadata !74, metadata !DIExpression()), !dbg !165
  %71 = load i32, ptr @sites_on_node, align 4, !dbg !344, !tbaa !233
  %72 = sext i32 %71 to i64, !dbg !344
  %cmp198 = icmp slt i64 %indvars.iv.next634, %72, !dbg !344
  br i1 %cmp198, label %for.body200, label %for.inc238, !dbg !347, !llvm.loop !355

for.inc238:                                       ; preds = %for.body200, %side_link_3f_force.exit, %for.body148
  %73 = phi <2 x float> [ %67, %side_link_3f_force.exit ], [ %64, %for.body148 ], [ %67, %for.body200 ]
  %inc239 = add nuw nsw i32 %rho.0618, 1, !dbg !357
  tail call void @llvm.dbg.value(metadata i32 %inc239, metadata !137, metadata !DIExpression()), !dbg !165
  %exitcond636.not = icmp eq i32 %inc239, 8, !dbg !358
  br i1 %exitcond636.not, label %for.end240, label %for.body148, !dbg !292, !llvm.loop !359

for.end240:                                       ; preds = %for.inc238
  tail call void @u_shift_hw_fermion(ptr noundef %call.6, ptr noundef %call.3, i32 noundef %nu.0622), !dbg !361
  call void @llvm.dbg.assign(metadata i1 undef, metadata !308, metadata !DIExpression(), metadata !162, metadata ptr %m_coeff.i560, metadata !DIExpression()), !dbg !362
  call void @llvm.dbg.value(metadata i32 %nu.0622, metadata !314, metadata !DIExpression()), !dbg !362
  call void @llvm.dbg.value(metadata i64 %indvars.iv648, metadata !315, metadata !DIExpression()), !dbg !362
  call void @llvm.dbg.value(metadata ptr %mFiveSt, metadata !316, metadata !DIExpression()), !dbg !362
  call void @llvm.dbg.value(metadata ptr %call, metadata !317, metadata !DIExpression()), !dbg !362
  call void @llvm.dbg.value(metadata ptr %call.6, metadata !318, metadata !DIExpression()), !dbg !362
  call void @llvm.dbg.value(metadata ptr %call.1, metadata !319, metadata !DIExpression()), !dbg !362
  call void @llvm.dbg.value(metadata ptr %call.3, metadata !320, metadata !DIExpression()), !dbg !362
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %m_coeff.i560) #8, !dbg !364
  call void @llvm.dbg.assign(metadata float undef, metadata !308, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !365, metadata ptr %m_coeff.i560, metadata !DIExpression()), !dbg !362
  %74 = load <2 x float>, ptr %mFiveSt, align 8, !dbg !366, !tbaa !193
  %75 = fneg <2 x float> %74, !dbg !367
  store <2 x float> %75, ptr %m_coeff.i560, align 8, !dbg !368, !tbaa !193, !DIAssignID !365
  call void @llvm.dbg.assign(metadata float undef, metadata !308, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !365, metadata ptr %m_coeff.i560, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !362
  %cmp.i565 = icmp ult i32 %nu.0622, 4, !dbg !369
  br i1 %cmp.i565, label %if.then.i571, label %if.else7.i567, !dbg !370

if.then.i571:                                     ; preds = %for.end240
  br i1 %cmp110, label %if.then6.i574, label %if.else.i572, !dbg !371

if.then6.i574:                                    ; preds = %if.then.i571
  call void @add_3f_force_to_mom(ptr noundef %call.3, ptr noundef %call, i32 noundef %nu.0622, ptr noundef nonnull %mFiveSt), !dbg !372
  br label %side_link_3f_force.exit575, !dbg !372

if.else.i572:                                     ; preds = %if.then.i571
  call void @add_3f_force_to_mom(ptr noundef %call, ptr noundef %call.3, i32 noundef %sub135, ptr noundef nonnull %m_coeff.i560), !dbg !373
  br label %side_link_3f_force.exit575

if.else7.i567:                                    ; preds = %for.end240
  br i1 %cmp110, label %if.then9.i570, label %if.else11.i568, !dbg !374

if.then9.i570:                                    ; preds = %if.else7.i567
  call void @add_3f_force_to_mom(ptr noundef %call.6, ptr noundef %call.1, i32 noundef %nu.0622, ptr noundef nonnull %m_coeff.i560), !dbg !375
  br label %side_link_3f_force.exit575, !dbg !375

if.else11.i568:                                   ; preds = %if.else7.i567
  call void @add_3f_force_to_mom(ptr noundef %call.1, ptr noundef %call.6, i32 noundef %sub135, ptr noundef nonnull %mFiveSt), !dbg !376
  br label %side_link_3f_force.exit575

side_link_3f_force.exit575:                       ; preds = %if.else11.i568, %if.then9.i570, %if.else.i572, %if.then6.i574
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %m_coeff.i560) #8, !dbg !377
  %div250 = fdiv float %61, %52, !dbg !275
  tail call void @llvm.dbg.value(metadata float %div250, metadata !143, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32)), !dbg !165
  %div254 = fdiv float %60, %53, !dbg !280
  tail call void @llvm.dbg.value(metadata float %div254, metadata !143, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32)), !dbg !165
  tail call void @llvm.dbg.value(metadata i32 0, metadata !73, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr poison, metadata !74, metadata !DIExpression()), !dbg !165
  %76 = load i32, ptr @sites_on_node, align 4, !dbg !378, !tbaa !233
  %cmp257620 = icmp sgt i32 %76, 0, !dbg !378
  br i1 %cmp257620, label %for.body259, label %for.inc297, !dbg !381

for.body259:                                      ; preds = %for.body259, %side_link_3f_force.exit575
  %indvars.iv637 = phi i64 [ %indvars.iv.next638, %for.body259 ], [ 0, %side_link_3f_force.exit575 ]
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv637, metadata !73, metadata !DIExpression()), !dbg !165
  %arrayidx262 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.7, i64 %indvars.iv637, !dbg !382
  %arrayidx267 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.3, i64 %indvars.iv637, !dbg !384
  tail call void @scalar_mult_add_su3_vector(ptr noundef %arrayidx262, ptr noundef %arrayidx267, float noundef %div250, ptr noundef %arrayidx262) #8, !dbg !385
  %arrayidx280 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.7, i64 %indvars.iv637, i32 0, i64 1, !dbg !386
  %arrayidx285 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.3, i64 %indvars.iv637, i32 0, i64 1, !dbg !387
  tail call void @scalar_mult_add_su3_vector(ptr noundef nonnull %arrayidx280, ptr noundef nonnull %arrayidx285, float noundef %div254, ptr noundef nonnull %arrayidx280) #8, !dbg !388
  %indvars.iv.next638 = add nuw nsw i64 %indvars.iv637, 1, !dbg !378
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv.next638, metadata !73, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr poison, metadata !74, metadata !DIExpression()), !dbg !165
  %77 = load i32, ptr @sites_on_node, align 4, !dbg !378, !tbaa !233
  %78 = sext i32 %77 to i64, !dbg !378
  %cmp257 = icmp slt i64 %indvars.iv.next638, %78, !dbg !378
  br i1 %cmp257, label %for.body259, label %for.inc297, !dbg !381, !llvm.loop !389

for.inc297:                                       ; preds = %for.body259, %side_link_3f_force.exit575, %for.body118
  %79 = phi float [ %60, %side_link_3f_force.exit575 ], [ %54, %for.body118 ], [ %60, %for.body259 ]
  %80 = phi float [ %61, %side_link_3f_force.exit575 ], [ %55, %for.body118 ], [ %61, %for.body259 ]
  %81 = phi float [ %60, %side_link_3f_force.exit575 ], [ %56, %for.body118 ], [ %60, %for.body259 ]
  %82 = phi float [ %61, %side_link_3f_force.exit575 ], [ %57, %for.body118 ], [ %61, %for.body259 ]
  %83 = phi <2 x float> [ %73, %side_link_3f_force.exit575 ], [ %58, %for.body118 ], [ %73, %for.body259 ]
  %inc298 = add nuw nsw i32 %nu.0622, 1, !dbg !391
  tail call void @llvm.dbg.value(metadata i32 %inc298, metadata !136, metadata !DIExpression()), !dbg !165
  %exitcond640.not = icmp eq i32 %inc298, 8, !dbg !392
  br i1 %exitcond640.not, label %for.end299, label %for.body118, !dbg !281, !llvm.loop !393

for.end299:                                       ; preds = %for.inc297
  tail call void @u_shift_hw_fermion(ptr noundef %call, ptr noundef %call.1, i32 noundef %sub107), !dbg !395
  tail call void @u_shift_hw_fermion(ptr noundef %call.1, ptr noundef %call.6, i32 noundef %43), !dbg !396
  br i1 %cmp110, label %if.then307, label %if.end311, !dbg !397

if.then307:                                       ; preds = %for.end299
  call void @add_3f_force_to_mom(ptr noundef %call.6, ptr noundef %call.1, i32 noundef %43, ptr noundef nonnull %Lepage), !dbg !398
  br label %if.end311, !dbg !401

if.end311:                                        ; preds = %if.then307, %for.end299
  tail call void @u_shift_hw_fermion(ptr noundef %call.6, ptr noundef %call.3, i32 noundef %50), !dbg !402
  call void @llvm.dbg.assign(metadata i1 undef, metadata !308, metadata !DIExpression(), metadata !161, metadata ptr %m_coeff.i576, metadata !DIExpression()), !dbg !403
  call void @llvm.dbg.value(metadata i64 %indvars.iv644, metadata !314, metadata !DIExpression()), !dbg !403
  call void @llvm.dbg.value(metadata i64 %indvars.iv648, metadata !315, metadata !DIExpression()), !dbg !403
  call void @llvm.dbg.value(metadata ptr %mLepage, metadata !316, metadata !DIExpression()), !dbg !403
  call void @llvm.dbg.value(metadata ptr %call, metadata !317, metadata !DIExpression()), !dbg !403
  call void @llvm.dbg.value(metadata ptr %call.6, metadata !318, metadata !DIExpression()), !dbg !403
  call void @llvm.dbg.value(metadata ptr %call.1, metadata !319, metadata !DIExpression()), !dbg !403
  call void @llvm.dbg.value(metadata ptr %call.3, metadata !320, metadata !DIExpression()), !dbg !403
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %m_coeff.i576) #8, !dbg !405
  call void @llvm.dbg.assign(metadata float undef, metadata !308, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !406, metadata ptr %m_coeff.i576, metadata !DIExpression()), !dbg !403
  %84 = load <2 x float>, ptr %mLepage, align 8, !dbg !407, !tbaa !193
  %85 = fneg <2 x float> %84, !dbg !408
  store <2 x float> %85, ptr %m_coeff.i576, align 8, !dbg !409, !tbaa !193, !DIAssignID !406
  call void @llvm.dbg.assign(metadata float undef, metadata !308, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !406, metadata ptr %m_coeff.i576, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !403
  %cmp.i581 = icmp ult i64 %indvars.iv644, 4, !dbg !410
  br i1 %cmp.i581, label %if.then.i587, label %if.else7.i583, !dbg !411

if.then.i587:                                     ; preds = %if.end311
  br i1 %cmp110, label %if.then6.i590, label %if.else.i588, !dbg !412

if.then6.i590:                                    ; preds = %if.then.i587
  call void @add_3f_force_to_mom(ptr noundef %call.3, ptr noundef %call, i32 noundef %50, ptr noundef nonnull %mLepage), !dbg !413
  br label %side_link_3f_force.exit591, !dbg !413

if.else.i588:                                     ; preds = %if.then.i587
  call void @add_3f_force_to_mom(ptr noundef %call, ptr noundef %call.3, i32 noundef %sub107, ptr noundef nonnull %m_coeff.i576), !dbg !414
  br label %side_link_3f_force.exit591

if.else7.i583:                                    ; preds = %if.end311
  br i1 %cmp110, label %if.then9.i586, label %if.else11.i584, !dbg !415

if.then9.i586:                                    ; preds = %if.else7.i583
  call void @add_3f_force_to_mom(ptr noundef %call.6, ptr noundef %call.1, i32 noundef %50, ptr noundef nonnull %m_coeff.i576), !dbg !416
  br label %side_link_3f_force.exit591, !dbg !416

if.else11.i584:                                   ; preds = %if.else7.i583
  call void @add_3f_force_to_mom(ptr noundef %call.1, ptr noundef %call.6, i32 noundef %sub107, ptr noundef nonnull %mLepage), !dbg !417
  br label %side_link_3f_force.exit591

side_link_3f_force.exit591:                       ; preds = %if.else11.i584, %if.then9.i586, %if.else.i588, %if.then6.i590
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %m_coeff.i576) #8, !dbg !418
  %86 = load float, ptr %Lepage, align 8, !dbg !419, !tbaa !193
  %div321 = fdiv float %86, %52, !dbg !420
  tail call void @llvm.dbg.value(metadata float %div321, metadata !143, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32)), !dbg !165
  %87 = load float, ptr %arrayidx58, align 4, !dbg !421, !tbaa !193
  %div325 = fdiv float %87, %53, !dbg !422
  tail call void @llvm.dbg.value(metadata float %div325, metadata !143, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32)), !dbg !165
  tail call void @llvm.dbg.value(metadata i32 0, metadata !73, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr poison, metadata !74, metadata !DIExpression()), !dbg !165
  %88 = load i32, ptr @sites_on_node, align 4, !dbg !423, !tbaa !233
  %cmp328624 = icmp sgt i32 %88, 0, !dbg !423
  br i1 %cmp328624, label %for.body330, label %for.end366, !dbg !426

for.body330:                                      ; preds = %for.body330, %side_link_3f_force.exit591
  %indvars.iv641 = phi i64 [ %indvars.iv.next642, %for.body330 ], [ 0, %side_link_3f_force.exit591 ]
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv641, metadata !73, metadata !DIExpression()), !dbg !165
  %arrayidx333 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.7, i64 %indvars.iv641, !dbg !427
  %arrayidx338 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.3, i64 %indvars.iv641, !dbg !429
  tail call void @scalar_mult_add_su3_vector(ptr noundef %arrayidx333, ptr noundef %arrayidx338, float noundef %div321, ptr noundef %arrayidx333) #8, !dbg !430
  %arrayidx351 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.7, i64 %indvars.iv641, i32 0, i64 1, !dbg !431
  %arrayidx356 = getelementptr inbounds %struct.half_wilson_vector, ptr %call.3, i64 %indvars.iv641, i32 0, i64 1, !dbg !432
  tail call void @scalar_mult_add_su3_vector(ptr noundef nonnull %arrayidx351, ptr noundef nonnull %arrayidx356, float noundef %div325, ptr noundef nonnull %arrayidx351) #8, !dbg !433
  %indvars.iv.next642 = add nuw nsw i64 %indvars.iv641, 1, !dbg !423
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv.next642, metadata !73, metadata !DIExpression()), !dbg !165
  tail call void @llvm.dbg.value(metadata ptr poison, metadata !74, metadata !DIExpression()), !dbg !165
  %89 = load i32, ptr @sites_on_node, align 4, !dbg !423, !tbaa !233
  %90 = sext i32 %89 to i64, !dbg !423
  %cmp328 = icmp slt i64 %indvars.iv.next642, %90, !dbg !423
  br i1 %cmp328, label %for.body330, label %for.end366, !dbg !426, !llvm.loop !434

for.end366:                                       ; preds = %for.body330, %side_link_3f_force.exit591
  br i1 %cmp.i581, label %if.end372.thread, label %if.end372, !dbg !436

if.end372.thread:                                 ; preds = %for.end366
  tail call void @u_shift_hw_fermion(ptr noundef %call.7, ptr noundef %call.3, i32 noundef %50), !dbg !437
  call void @llvm.dbg.assign(metadata i1 undef, metadata !308, metadata !DIExpression(), metadata !160, metadata ptr %m_coeff.i592, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata i64 %indvars.iv644, metadata !314, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata i64 %indvars.iv648, metadata !315, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata ptr %ThreeSt, metadata !316, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata ptr %call77, metadata !317, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata ptr %call.7, metadata !318, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata ptr %call, metadata !319, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata ptr %call.3, metadata !320, metadata !DIExpression()), !dbg !439
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %m_coeff.i592) #8, !dbg !441
  %91 = fneg <2 x float> %51, !dbg !442
  call void @llvm.dbg.assign(metadata float undef, metadata !308, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !443, metadata ptr %m_coeff.i592, metadata !DIExpression()), !dbg !439
  store <2 x float> %91, ptr %m_coeff.i592, align 8, !dbg !444, !tbaa !193, !DIAssignID !443
  call void @llvm.dbg.assign(metadata float undef, metadata !308, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !443, metadata ptr %m_coeff.i592, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !439
  br i1 %cmp110, label %if.then6.i606, label %if.else.i604, !dbg !445

if.end372:                                        ; preds = %for.end366
  call void @llvm.dbg.assign(metadata i1 undef, metadata !308, metadata !DIExpression(), metadata !160, metadata ptr %m_coeff.i592, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata i64 %indvars.iv644, metadata !314, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata i64 %indvars.iv648, metadata !315, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata ptr %ThreeSt, metadata !316, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata ptr %call77, metadata !317, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata ptr %call.7, metadata !318, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata ptr %call, metadata !319, metadata !DIExpression()), !dbg !439
  call void @llvm.dbg.value(metadata ptr %call.3, metadata !320, metadata !DIExpression()), !dbg !439
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %m_coeff.i592) #8, !dbg !441
  call void @llvm.dbg.assign(metadata float undef, metadata !308, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 32), metadata !443, metadata ptr %m_coeff.i592, metadata !DIExpression()), !dbg !439
  %92 = fneg <2 x float> %51, !dbg !442
  store <2 x float> %92, ptr %m_coeff.i592, align 8, !dbg !444, !tbaa !193, !DIAssignID !443
  call void @llvm.dbg.assign(metadata float undef, metadata !308, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 32), metadata !443, metadata ptr %m_coeff.i592, metadata !DIExpression(DW_OP_plus_uconst, 4)), !dbg !439
  br i1 %cmp110, label %if.then9.i602, label %if.else11.i600, !dbg !446

if.then6.i606:                                    ; preds = %if.end372.thread
  call void @add_3f_force_to_mom(ptr noundef %call.3, ptr noundef %call77, i32 noundef %50, ptr noundef nonnull %ThreeSt), !dbg !447
  br label %side_link_3f_force.exit607, !dbg !447

if.else.i604:                                     ; preds = %if.end372.thread
  call void @add_3f_force_to_mom(ptr noundef %call77, ptr noundef %call.3, i32 noundef %sub107, ptr noundef nonnull %m_coeff.i592), !dbg !448
  br label %side_link_3f_force.exit607

if.then9.i602:                                    ; preds = %if.end372
  call void @add_3f_force_to_mom(ptr noundef %call.7, ptr noundef %call, i32 noundef %50, ptr noundef nonnull %m_coeff.i592), !dbg !449
  br label %side_link_3f_force.exit607, !dbg !449

if.else11.i600:                                   ; preds = %if.end372
  call void @add_3f_force_to_mom(ptr noundef %call, ptr noundef %call.7, i32 noundef %sub107, ptr noundef nonnull %ThreeSt), !dbg !450
  br label %side_link_3f_force.exit607

side_link_3f_force.exit607:                       ; preds = %if.else11.i600, %if.then9.i602, %if.else.i604, %if.then6.i606
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %m_coeff.i592) #8, !dbg !451
  %arrayidx378 = getelementptr inbounds [8 x i32], ptr %DirectLinks, i64 0, i64 %indvars.iv644, !dbg !452
  %93 = load i32, ptr %arrayidx378, align 4, !dbg !452, !tbaa !233
  %tobool.not = icmp eq i32 %93, 0, !dbg !452
  br i1 %tobool.not, label %if.then379, label %for.inc406, !dbg !454

if.then379:                                       ; preds = %side_link_3f_force.exit607
  store i32 1, ptr %arrayidx378, align 4, !dbg !455, !tbaa !233
  %cmp382 = icmp ugt i64 %indvars.iv644, 3, !dbg !457
  br i1 %cmp382, label %if.then384, label %if.else, !dbg !459

if.then384:                                       ; preds = %if.then379
  call void @add_3f_force_to_mom(ptr noundef %call, ptr noundef %call77, i32 noundef %sub107, ptr noundef nonnull %OneLink), !dbg !460
  tail call void @u_shift_hw_fermion(ptr noundef %call77, ptr noundef %call.4, i32 noundef %50), !dbg !462
  call void @add_3f_force_to_mom(ptr noundef %call.1, ptr noundef %call.4, i32 noundef %sub107, ptr noundef nonnull %mNaik), !dbg !463
  tail call void @u_shift_hw_fermion(ptr noundef %call.1, ptr noundef %call.4, i32 noundef %sub107), !dbg !464
  call void @add_3f_force_to_mom(ptr noundef %call.4, ptr noundef %call77, i32 noundef %sub107, ptr noundef nonnull %Naik), !dbg !465
  br label %for.inc406, !dbg !466

if.else:                                          ; preds = %if.then379
  tail call void @u_shift_hw_fermion(ptr noundef %call77, ptr noundef %call.4, i32 noundef %50), !dbg !467
  call void @add_3f_force_to_mom(ptr noundef %call.4, ptr noundef %call.1, i32 noundef %50, ptr noundef nonnull %Naik), !dbg !469
  br label %for.inc406

for.inc406:                                       ; preds = %if.else, %if.then384, %side_link_3f_force.exit607, %for.body101
  %94 = phi float [ %45, %for.body101 ], [ %79, %if.then384 ], [ %79, %if.else ], [ %79, %side_link_3f_force.exit607 ]
  %95 = phi float [ %46, %for.body101 ], [ %80, %if.then384 ], [ %80, %if.else ], [ %80, %side_link_3f_force.exit607 ]
  %96 = phi float [ %47, %for.body101 ], [ %81, %if.then384 ], [ %81, %if.else ], [ %81, %side_link_3f_force.exit607 ]
  %97 = phi float [ %48, %for.body101 ], [ %82, %if.then384 ], [ %82, %if.else ], [ %82, %side_link_3f_force.exit607 ]
  %98 = phi <2 x float> [ %49, %for.body101 ], [ %83, %if.then384 ], [ %83, %if.else ], [ %83, %side_link_3f_force.exit607 ]
  %indvars.iv.next645 = add nuw nsw i64 %indvars.iv644, 1, !dbg !470
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv.next645, metadata !135, metadata !DIExpression()), !dbg !165
  %exitcond647.not = icmp eq i64 %indvars.iv.next645, 8, !dbg !471
  br i1 %exitcond647.not, label %for.inc409, label %for.body101, !dbg !258, !llvm.loop !472

for.inc409:                                       ; preds = %for.inc406
  %indvars.iv.next649 = add nuw nsw i64 %indvars.iv648, 1, !dbg !474
  tail call void @llvm.dbg.value(metadata i64 %indvars.iv.next649, metadata !138, metadata !DIExpression()), !dbg !165
  %exitcond651.not = icmp eq i64 %indvars.iv.next649, 8, !dbg !475
  br i1 %exitcond651.not, label %for.end411, label %for.cond98.preheader, !dbg !254, !llvm.loop !476

for.end411:                                       ; preds = %for.inc409
  tail call void @free(ptr noundef %call77) #8, !dbg !478
  tail call void @llvm.dbg.value(metadata i64 0, metadata !135, metadata !DIExpression()), !dbg !165
  tail call void @free(ptr noundef %call) #8, !dbg !479
  tail call void @llvm.dbg.value(metadata i64 1, metadata !135, metadata !DIExpression()), !dbg !165
  tail call void @free(ptr noundef %call.1) #8, !dbg !479
  tail call void @llvm.dbg.value(metadata i64 2, metadata !135, metadata !DIExpression()), !dbg !165
  tail call void @free(ptr noundef %call.2) #8, !dbg !479
  tail call void @llvm.dbg.value(metadata i64 3, metadata !135, metadata !DIExpression()), !dbg !165
  tail call void @free(ptr noundef %call.3) #8, !dbg !479
  tail call void @llvm.dbg.value(metadata i64 4, metadata !135, metadata !DIExpression()), !dbg !165
  tail call void @free(ptr noundef %call.4) #8, !dbg !479
  tail call void @llvm.dbg.value(metadata i64 6, metadata !135, metadata !DIExpression()), !dbg !165
  tail call void @free(ptr noundef %call.6) #8, !dbg !479
  tail call void @llvm.dbg.value(metadata i64 7, metadata !135, metadata !DIExpression()), !dbg !165
  tail call void @free(ptr noundef %call.7) #8, !dbg !479
  tail call void @llvm.dbg.value(metadata i64 8, metadata !135, metadata !DIExpression()), !dbg !165
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %mSevenSt) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %mThreeSt) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %mFiveSt) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %mLepage) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %mNaik) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %SevenSt) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %ThreeSt) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %FiveSt) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %Naik) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %Lepage) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %OneLink) #8, !dbg !482
  call void @llvm.lifetime.end.p0(i64 32, ptr nonnull %DirectLinks) #8, !dbg !482
  ret void, !dbg !482
}

; Function Attrs: nounwind uwtable
declare dso_local void @u_shift_hw_fermion(ptr noundef, ptr noundef, i32 noundef) local_unnamed_addr #5

; Function Attrs: nounwind uwtable
declare dso_local void @add_3f_force_to_mom(ptr noundef, ptr noundef, i32 noundef, ptr nocapture noundef) local_unnamed_addr #5

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.assign(metadata, metadata, metadata, metadata, metadata, metadata) #6

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #6

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #7

attributes #0 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #1 = { mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #3 = { mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #7 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #8 = { nounwind }
attributes #9 = { nounwind allocsize(0) }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!48, !49, !50, !51, !52, !53, !54}
!llvm.ident = !{!55}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "act_path_coeff", scope: !2, file: !45, line: 145, type: !46, isLocal: true, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C89, file: !3, producer: "clang version 18.1.8", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !4, globals: !41, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "/home/ahueck/workspace/dimeta-tests/testsuite/miniapp/milc/quark_stuff.c", directory: "/home/ahueck/workspace/dimeta-tests/build", checksumkind: CSK_MD5, checksum: "93ceb7c7f928462e7695f7b878049152")
!4 = !{!5, !21, !23, !31, !34}
!5 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!6 = !DIDerivedType(tag: DW_TAG_typedef, name: "su3_vector", file: !7, line: 15, baseType: !8)
!7 = !DIFile(filename: "testsuite/miniapp/milc/include/su3.h", directory: "/home/ahueck/workspace/dimeta-tests", checksumkind: CSK_MD5, checksum: "31997d5e7a0a485a983d1913a2140422")
!8 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !7, line: 15, size: 192, elements: !9)
!9 = !{!10}
!10 = !DIDerivedType(tag: DW_TAG_member, name: "c", scope: !8, file: !7, line: 15, baseType: !11, size: 192)
!11 = !DICompositeType(tag: DW_TAG_array_type, baseType: !12, size: 192, elements: !19)
!12 = !DIDerivedType(tag: DW_TAG_typedef, name: "complex", file: !13, line: 76, baseType: !14)
!13 = !DIFile(filename: "testsuite/miniapp/milc/include/complex.h", directory: "/home/ahueck/workspace/dimeta-tests", checksumkind: CSK_MD5, checksum: "fea3aa9a1e9294803e3e0b191e32c97f")
!14 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !13, line: 73, size: 64, elements: !15)
!15 = !{!16, !18}
!16 = !DIDerivedType(tag: DW_TAG_member, name: "real", scope: !14, file: !13, line: 74, baseType: !17, size: 32)
!17 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!18 = !DIDerivedType(tag: DW_TAG_member, name: "imag", scope: !14, file: !13, line: 75, baseType: !17, size: 32, offset: 32)
!19 = !{!20}
!20 = !DISubrange(count: 3)
!21 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !22, size: 64)
!22 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = !DIDerivedType(tag: DW_TAG_typedef, name: "half_wilson_vector", file: !7, line: 47, baseType: !25)
!25 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !7, line: 47, size: 384, elements: !26)
!26 = !{!27}
!27 = !DIDerivedType(tag: DW_TAG_member, name: "h", scope: !25, file: !7, line: 47, baseType: !28, size: 384)
!28 = !DICompositeType(tag: DW_TAG_array_type, baseType: !6, size: 384, elements: !29)
!29 = !{!30}
!30 = !DISubrange(count: 2)
!31 = !DIDerivedType(tag: DW_TAG_typedef, name: "field_offset", file: !32, line: 42, baseType: !33)
!32 = !DIFile(filename: "testsuite/miniapp/milc/include/macros.h", directory: "/home/ahueck/workspace/dimeta-tests", checksumkind: CSK_MD5, checksum: "e66956ca749f7c5112a24ffaf7306f75")
!33 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!34 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !35, size: 64)
!35 = !DIDerivedType(tag: DW_TAG_typedef, name: "su3_matrix", file: !7, line: 14, baseType: !36)
!36 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !7, line: 14, size: 576, elements: !37)
!37 = !{!38}
!38 = !DIDerivedType(tag: DW_TAG_member, name: "e", scope: !36, file: !7, line: 14, baseType: !39, size: 576)
!39 = !DICompositeType(tag: DW_TAG_array_type, baseType: !12, size: 576, elements: !40)
!40 = !{!20, !20}
!41 = !{!0, !42}
!42 = !DIGlobalVariableExpression(var: !43, expr: !DIExpression(DW_OP_constu, 6, DW_OP_stack_value))
!43 = distinct !DIGlobalVariable(name: "quark_action_npaths", scope: !2, file: !44, line: 31, type: !33, isLocal: true, isDefinition: true)
!44 = !DIFile(filename: "testsuite/miniapp/milc/include/quark_action.h", directory: "/home/ahueck/workspace/dimeta-tests", checksumkind: CSK_MD5, checksum: "4ad0ad963f26e4621ecebbe42a0335e1")
!45 = !DIFile(filename: "testsuite/miniapp/milc/quark_stuff.c", directory: "/home/ahueck/workspace/dimeta-tests", checksumkind: CSK_MD5, checksum: "93ceb7c7f928462e7695f7b878049152")
!46 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 192, elements: !47)
!47 = !{}
!48 = !{i32 7, !"Dwarf Version", i32 5}
!49 = !{i32 2, !"Debug Info Version", i32 3}
!50 = !{i32 1, !"wchar_size", i32 4}
!51 = !{i32 8, !"PIC Level", i32 2}
!52 = !{i32 7, !"PIE Level", i32 2}
!53 = !{i32 7, !"uwtable", i32 2}
!54 = !{i32 7, !"debug-info-assignment-tracking", i1 true}
!55 = !{!"clang version 18.1.8"}
!56 = !DISubprogram(name: "malloc", scope: !57, file: !57, line: 672, type: !58, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized)
!57 = !DIFile(filename: "/usr/include/stdlib.h", directory: "", checksumkind: CSK_MD5, checksum: "7fa2ecb2348a66f8b44ab9a15abd0b72")
!58 = !DISubroutineType(types: !47)
!59 = !DISubprogram(name: "free", scope: !57, file: !57, line: 687, type: !60, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized)
!60 = distinct !DISubroutineType(types: !61)
!61 = !{null}
!62 = !DISubprogram(name: "scalar_mult_add_su3_vector", scope: !7, file: !7, line: 415, type: !63, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized)
!63 = !DISubroutineType(types: !64)
!64 = !{null, !5, !5, !17, !5}
!65 = distinct !DISubprogram(name: "eo_fermion_force_3f", scope: !45, file: !45, line: 1236, type: !66, scopeLine: 1237, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !67)
!66 = !DISubroutineType(types: !61)
!67 = !{!68, !69, !70, !71, !72, !73, !74, !135, !136, !137, !138, !139, !143, !145, !146, !147, !148, !149, !150, !151, !152, !153, !154, !155, !156, !157, !159}
!68 = !DILocalVariable(name: "eps", arg: 1, scope: !65, file: !45, line: 1236, type: !17)
!69 = !DILocalVariable(name: "nflav1", arg: 2, scope: !65, file: !45, line: 1236, type: !33)
!70 = !DILocalVariable(name: "x1_off", arg: 3, scope: !65, file: !45, line: 1236, type: !31)
!71 = !DILocalVariable(name: "nflav2", arg: 4, scope: !65, file: !45, line: 1236, type: !33)
!72 = !DILocalVariable(name: "x2_off", arg: 5, scope: !65, file: !45, line: 1237, type: !31)
!73 = !DILocalVariable(name: "i", scope: !65, file: !45, line: 1243, type: !33)
!74 = !DILocalVariable(name: "s", scope: !65, file: !45, line: 1244, type: !75)
!75 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !76, size: 64)
!76 = !DIDerivedType(tag: DW_TAG_typedef, name: "site", file: !77, line: 99, baseType: !78)
!77 = !DIFile(filename: "testsuite/miniapp/milc/include/lattice.h", directory: "/home/ahueck/workspace/dimeta-tests", checksumkind: CSK_MD5, checksum: "b28578d741f5c0e1a18d21b2de13e17c")
!78 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !77, line: 31, size: 8640, elements: !79)
!79 = !{!80, !82, !83, !84, !85, !86, !87, !104, !105, !109, !121, !123, !124, !125, !126, !127, !128, !129, !131, !132, !133, !134}
!80 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !78, file: !77, line: 34, baseType: !81, size: 16)
!81 = !DIBasicType(name: "short", size: 16, encoding: DW_ATE_signed)
!82 = !DIDerivedType(tag: DW_TAG_member, name: "y", scope: !78, file: !77, line: 34, baseType: !81, size: 16, offset: 16)
!83 = !DIDerivedType(tag: DW_TAG_member, name: "z", scope: !78, file: !77, line: 34, baseType: !81, size: 16, offset: 32)
!84 = !DIDerivedType(tag: DW_TAG_member, name: "t", scope: !78, file: !77, line: 34, baseType: !81, size: 16, offset: 48)
!85 = !DIDerivedType(tag: DW_TAG_member, name: "parity", scope: !78, file: !77, line: 36, baseType: !22, size: 8, offset: 64)
!86 = !DIDerivedType(tag: DW_TAG_member, name: "index", scope: !78, file: !77, line: 38, baseType: !33, size: 32, offset: 96)
!87 = !DIDerivedType(tag: DW_TAG_member, name: "site_prn", scope: !78, file: !77, line: 41, baseType: !88, size: 704, offset: 128)
!88 = !DIDerivedType(tag: DW_TAG_typedef, name: "double_prn", file: !89, line: 11, baseType: !90)
!89 = !DIFile(filename: "testsuite/miniapp/milc/include/random.h", directory: "/home/ahueck/workspace/dimeta-tests", checksumkind: CSK_MD5, checksum: "914e17056f9ae4b421a6226dd1019dca")
!90 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !89, line: 6, size: 704, elements: !91)
!91 = !{!92, !94, !95, !96, !97, !98, !99, !100, !101, !102, !103}
!92 = !DIDerivedType(tag: DW_TAG_member, name: "r0", scope: !90, file: !89, line: 8, baseType: !93, size: 64)
!93 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!94 = !DIDerivedType(tag: DW_TAG_member, name: "r1", scope: !90, file: !89, line: 8, baseType: !93, size: 64, offset: 64)
!95 = !DIDerivedType(tag: DW_TAG_member, name: "r2", scope: !90, file: !89, line: 8, baseType: !93, size: 64, offset: 128)
!96 = !DIDerivedType(tag: DW_TAG_member, name: "r3", scope: !90, file: !89, line: 8, baseType: !93, size: 64, offset: 192)
!97 = !DIDerivedType(tag: DW_TAG_member, name: "r4", scope: !90, file: !89, line: 8, baseType: !93, size: 64, offset: 256)
!98 = !DIDerivedType(tag: DW_TAG_member, name: "r5", scope: !90, file: !89, line: 8, baseType: !93, size: 64, offset: 320)
!99 = !DIDerivedType(tag: DW_TAG_member, name: "r6", scope: !90, file: !89, line: 8, baseType: !93, size: 64, offset: 384)
!100 = !DIDerivedType(tag: DW_TAG_member, name: "multiplier", scope: !90, file: !89, line: 9, baseType: !93, size: 64, offset: 448)
!101 = !DIDerivedType(tag: DW_TAG_member, name: "addend", scope: !90, file: !89, line: 9, baseType: !93, size: 64, offset: 512)
!102 = !DIDerivedType(tag: DW_TAG_member, name: "ic_state", scope: !90, file: !89, line: 9, baseType: !93, size: 64, offset: 576)
!103 = !DIDerivedType(tag: DW_TAG_member, name: "scale", scope: !90, file: !89, line: 10, baseType: !17, size: 32, offset: 640)
!104 = !DIDerivedType(tag: DW_TAG_member, name: "space1", scope: !78, file: !77, line: 43, baseType: !33, size: 32, offset: 832)
!105 = !DIDerivedType(tag: DW_TAG_member, name: "link", scope: !78, file: !77, line: 50, baseType: !106, size: 2304, offset: 864)
!106 = !DICompositeType(tag: DW_TAG_array_type, baseType: !35, size: 2304, elements: !107)
!107 = !{!108}
!108 = !DISubrange(count: 4)
!109 = !DIDerivedType(tag: DW_TAG_member, name: "mom", scope: !78, file: !77, line: 62, baseType: !110, size: 1280, offset: 3168)
!110 = !DICompositeType(tag: DW_TAG_array_type, baseType: !111, size: 1280, elements: !107)
!111 = !DIDerivedType(tag: DW_TAG_typedef, name: "anti_hermitmat", file: !7, line: 17, baseType: !112)
!112 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !7, line: 16, size: 320, elements: !113)
!113 = !{!114, !115, !116, !117, !118, !119, !120}
!114 = !DIDerivedType(tag: DW_TAG_member, name: "m01", scope: !112, file: !7, line: 17, baseType: !12, size: 64)
!115 = !DIDerivedType(tag: DW_TAG_member, name: "m02", scope: !112, file: !7, line: 17, baseType: !12, size: 64, offset: 64)
!116 = !DIDerivedType(tag: DW_TAG_member, name: "m12", scope: !112, file: !7, line: 17, baseType: !12, size: 64, offset: 128)
!117 = !DIDerivedType(tag: DW_TAG_member, name: "m00im", scope: !112, file: !7, line: 17, baseType: !17, size: 32, offset: 192)
!118 = !DIDerivedType(tag: DW_TAG_member, name: "m11im", scope: !112, file: !7, line: 17, baseType: !17, size: 32, offset: 224)
!119 = !DIDerivedType(tag: DW_TAG_member, name: "m22im", scope: !112, file: !7, line: 17, baseType: !17, size: 32, offset: 256)
!120 = !DIDerivedType(tag: DW_TAG_member, name: "space", scope: !112, file: !7, line: 17, baseType: !17, size: 32, offset: 288)
!121 = !DIDerivedType(tag: DW_TAG_member, name: "phase", scope: !78, file: !77, line: 66, baseType: !122, size: 128, offset: 4448)
!122 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 128, elements: !107)
!123 = !DIDerivedType(tag: DW_TAG_member, name: "phi", scope: !78, file: !77, line: 69, baseType: !6, size: 192, offset: 4576)
!124 = !DIDerivedType(tag: DW_TAG_member, name: "resid", scope: !78, file: !77, line: 70, baseType: !6, size: 192, offset: 4768)
!125 = !DIDerivedType(tag: DW_TAG_member, name: "cg_p", scope: !78, file: !77, line: 71, baseType: !6, size: 192, offset: 4960)
!126 = !DIDerivedType(tag: DW_TAG_member, name: "xxx", scope: !78, file: !77, line: 72, baseType: !6, size: 192, offset: 5152)
!127 = !DIDerivedType(tag: DW_TAG_member, name: "ttt", scope: !78, file: !77, line: 73, baseType: !6, size: 192, offset: 5344)
!128 = !DIDerivedType(tag: DW_TAG_member, name: "g_rand", scope: !78, file: !77, line: 74, baseType: !6, size: 192, offset: 5536)
!129 = !DIDerivedType(tag: DW_TAG_member, name: "tempvec", scope: !78, file: !77, line: 93, baseType: !130, size: 768, offset: 5728)
!130 = !DICompositeType(tag: DW_TAG_array_type, baseType: !6, size: 768, elements: !107)
!131 = !DIDerivedType(tag: DW_TAG_member, name: "templongvec", scope: !78, file: !77, line: 95, baseType: !130, size: 768, offset: 6496)
!132 = !DIDerivedType(tag: DW_TAG_member, name: "templongv1", scope: !78, file: !77, line: 96, baseType: !6, size: 192, offset: 7264)
!133 = !DIDerivedType(tag: DW_TAG_member, name: "tempmat1", scope: !78, file: !77, line: 98, baseType: !35, size: 576, offset: 7456)
!134 = !DIDerivedType(tag: DW_TAG_member, name: "staple", scope: !78, file: !77, line: 98, baseType: !35, size: 576, offset: 8032)
!135 = !DILocalVariable(name: "mu", scope: !65, file: !45, line: 1245, type: !33)
!136 = !DILocalVariable(name: "nu", scope: !65, file: !45, line: 1245, type: !33)
!137 = !DILocalVariable(name: "rho", scope: !65, file: !45, line: 1245, type: !33)
!138 = !DILocalVariable(name: "sig", scope: !65, file: !45, line: 1245, type: !33)
!139 = !DILocalVariable(name: "DirectLinks", scope: !65, file: !45, line: 1246, type: !140)
!140 = !DICompositeType(tag: DW_TAG_array_type, baseType: !33, size: 256, elements: !141)
!141 = !{!142}
!142 = !DISubrange(count: 8)
!143 = !DILocalVariable(name: "coeff", scope: !65, file: !45, line: 1247, type: !144)
!144 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 64, elements: !29)
!145 = !DILocalVariable(name: "ferm_epsilon", scope: !65, file: !45, line: 1247, type: !17)
!146 = !DILocalVariable(name: "OneLink", scope: !65, file: !45, line: 1248, type: !144)
!147 = !DILocalVariable(name: "Lepage", scope: !65, file: !45, line: 1248, type: !144)
!148 = !DILocalVariable(name: "Naik", scope: !65, file: !45, line: 1248, type: !144)
!149 = !DILocalVariable(name: "FiveSt", scope: !65, file: !45, line: 1248, type: !144)
!150 = !DILocalVariable(name: "ThreeSt", scope: !65, file: !45, line: 1248, type: !144)
!151 = !DILocalVariable(name: "SevenSt", scope: !65, file: !45, line: 1248, type: !144)
!152 = !DILocalVariable(name: "mNaik", scope: !65, file: !45, line: 1249, type: !144)
!153 = !DILocalVariable(name: "mLepage", scope: !65, file: !45, line: 1249, type: !144)
!154 = !DILocalVariable(name: "mFiveSt", scope: !65, file: !45, line: 1249, type: !144)
!155 = !DILocalVariable(name: "mThreeSt", scope: !65, file: !45, line: 1249, type: !144)
!156 = !DILocalVariable(name: "mSevenSt", scope: !65, file: !45, line: 1249, type: !144)
!157 = !DILocalVariable(name: "hwvec", scope: !65, file: !45, line: 1250, type: !158)
!158 = !DICompositeType(tag: DW_TAG_array_type, baseType: !23, size: 512, elements: !141)
!159 = !DILocalVariable(name: "temp_x", scope: !65, file: !45, line: 1251, type: !23)
!160 = distinct !DIAssignID()
!161 = distinct !DIAssignID()
!162 = distinct !DIAssignID()
!163 = distinct !DIAssignID()
!164 = distinct !DIAssignID()
!165 = !DILocation(line: 0, scope: !65)
!166 = distinct !DIAssignID()
!167 = distinct !DIAssignID()
!168 = distinct !DIAssignID()
!169 = distinct !DIAssignID()
!170 = distinct !DIAssignID()
!171 = distinct !DIAssignID()
!172 = distinct !DIAssignID()
!173 = distinct !DIAssignID()
!174 = distinct !DIAssignID()
!175 = distinct !DIAssignID()
!176 = distinct !DIAssignID()
!177 = !DILocation(line: 1246, column: 3, scope: !65)
!178 = !DILocation(line: 1248, column: 3, scope: !65)
!179 = !DILocation(line: 1249, column: 3, scope: !65)
!180 = !DILocation(line: 1273, column: 25, scope: !65)
!181 = !DILocation(line: 1260, column: 41, scope: !65)
!182 = !DILocation(line: 1261, column: 16, scope: !65)
!183 = distinct !DIAssignID()
!184 = !DILocation(line: 1262, column: 13, scope: !65)
!185 = distinct !DIAssignID()
!186 = distinct !DIAssignID()
!187 = !DILocation(line: 1264, column: 16, scope: !65)
!188 = distinct !DIAssignID()
!189 = distinct !DIAssignID()
!190 = distinct !DIAssignID()
!191 = distinct !DIAssignID()
!192 = !DILocation(line: 1268, column: 16, scope: !65)
!193 = !{!194, !194, i64 0}
!194 = !{!"float", !195, i64 0}
!195 = !{!"omnipotent char", !196, i64 0}
!196 = !{!"Simple C/C++ TBAA"}
!197 = distinct !DIAssignID()
!198 = distinct !DIAssignID()
!199 = !DILocation(line: 1270, column: 15, scope: !65)
!200 = distinct !DIAssignID()
!201 = distinct !DIAssignID()
!202 = !DILocation(line: 1279, column: 3, scope: !65)
!203 = !DILocation(line: 1273, column: 32, scope: !65)
!204 = !DILocation(line: 1273, column: 22, scope: !65)
!205 = !DILocation(line: 1273, column: 39, scope: !65)
!206 = !DILocation(line: 1273, column: 18, scope: !65)
!207 = !DILocation(line: 1281, column: 34, scope: !65)
!208 = !DILocation(line: 1261, column: 34, scope: !65)
!209 = !DILocation(line: 1261, column: 14, scope: !65)
!210 = !DILocation(line: 1262, column: 31, scope: !65)
!211 = !DILocation(line: 1262, column: 11, scope: !65)
!212 = !DILocation(line: 1263, column: 14, scope: !65)
!213 = !DILocation(line: 1263, column: 12, scope: !65)
!214 = !DILocation(line: 1264, column: 34, scope: !65)
!215 = !DILocation(line: 1264, column: 14, scope: !65)
!216 = !DILocation(line: 1265, column: 17, scope: !65)
!217 = !DILocation(line: 1265, column: 15, scope: !65)
!218 = !DILocation(line: 1266, column: 33, scope: !65)
!219 = !DILocation(line: 1266, column: 13, scope: !65)
!220 = !DILocation(line: 1267, column: 16, scope: !65)
!221 = !DILocation(line: 1267, column: 14, scope: !65)
!222 = !DILocation(line: 1268, column: 14, scope: !65)
!223 = !DILocation(line: 1269, column: 17, scope: !65)
!224 = !DILocation(line: 1269, column: 15, scope: !65)
!225 = !DILocation(line: 1283, column: 3, scope: !65)
!226 = !DILocation(line: 1270, column: 33, scope: !65)
!227 = !DILocation(line: 1270, column: 13, scope: !65)
!228 = !DILocation(line: 1271, column: 16, scope: !65)
!229 = !DILocation(line: 1271, column: 14, scope: !65)
!230 = !DILocation(line: 1289, column: 21, scope: !231)
!231 = distinct !DILexicalBlock(scope: !232, file: !45, line: 1288, column: 3)
!232 = distinct !DILexicalBlock(scope: !65, file: !45, line: 1288, column: 3)
!233 = !{!234, !234, i64 0}
!234 = !{!"int", !195, i64 0}
!235 = !DILocation(line: 1293, column: 39, scope: !236)
!236 = distinct !DILexicalBlock(scope: !237, file: !45, line: 1292, column: 3)
!237 = distinct !DILexicalBlock(scope: !65, file: !45, line: 1292, column: 3)
!238 = !DILocation(line: 1298, column: 29, scope: !65)
!239 = !DILocation(line: 1299, column: 3, scope: !240)
!240 = distinct !DILexicalBlock(scope: !241, file: !45, line: 1299, column: 3)
!241 = distinct !DILexicalBlock(scope: !65, file: !45, line: 1299, column: 3)
!242 = !DILocation(line: 1299, column: 3, scope: !241)
!243 = !{!244, !244, i64 0}
!244 = !{!"any pointer", !195, i64 0}
!245 = !DILocation(line: 1300, column: 5, scope: !246)
!246 = distinct !DILexicalBlock(scope: !240, file: !45, line: 1299, column: 21)
!247 = !DILocation(line: 1300, column: 37, scope: !246)
!248 = !DILocation(line: 1300, column: 22, scope: !246)
!249 = !{i64 0, i64 24, !250}
!250 = !{!195, !195, i64 0}
!251 = !DILocation(line: 1301, column: 5, scope: !246)
!252 = !DILocation(line: 1301, column: 37, scope: !246)
!253 = !DILocation(line: 1301, column: 22, scope: !246)
!254 = !DILocation(line: 1304, column: 3, scope: !255)
!255 = distinct !DILexicalBlock(scope: !65, file: !45, line: 1304, column: 3)
!256 = distinct !{!256, !242, !257}
!257 = !DILocation(line: 1302, column: 3, scope: !241)
!258 = !DILocation(line: 1305, column: 5, scope: !259)
!259 = distinct !DILexicalBlock(scope: !260, file: !45, line: 1305, column: 5)
!260 = distinct !DILexicalBlock(scope: !261, file: !45, line: 1304, column: 33)
!261 = distinct !DILexicalBlock(scope: !255, file: !45, line: 1304, column: 3)
!262 = !DILocation(line: 1306, column: 15, scope: !263)
!263 = distinct !DILexicalBlock(scope: !264, file: !45, line: 1306, column: 11)
!264 = distinct !DILexicalBlock(scope: !259, file: !45, line: 1305, column: 5)
!265 = !DILocation(line: 1306, column: 23, scope: !263)
!266 = !DILocation(line: 1307, column: 41, scope: !267)
!267 = distinct !DILexicalBlock(scope: !263, file: !45, line: 1306, column: 48)
!268 = !DILocation(line: 1307, column: 9, scope: !267)
!269 = !DILocation(line: 1308, column: 9, scope: !267)
!270 = !DILocation(line: 1309, column: 13, scope: !267)
!271 = !DILocation(line: 1314, column: 11, scope: !272)
!272 = distinct !DILexicalBlock(scope: !273, file: !45, line: 1309, column: 33)
!273 = distinct !DILexicalBlock(scope: !267, file: !45, line: 1309, column: 13)
!274 = !DILocation(line: 1315, column: 9, scope: !272)
!275 = !DILocation(line: 1361, column: 34, scope: !276)
!276 = distinct !DILexicalBlock(scope: !277, file: !45, line: 1318, column: 37)
!277 = distinct !DILexicalBlock(scope: !278, file: !45, line: 1317, column: 15)
!278 = distinct !DILexicalBlock(scope: !279, file: !45, line: 1316, column: 9)
!279 = distinct !DILexicalBlock(scope: !267, file: !45, line: 1316, column: 9)
!280 = !DILocation(line: 1362, column: 34, scope: !276)
!281 = !DILocation(line: 1316, column: 9, scope: !279)
!282 = !DILocation(line: 1317, column: 19, scope: !277)
!283 = !DILocation(line: 1317, column: 26, scope: !277)
!284 = !DILocation(line: 1319, column: 44, scope: !276)
!285 = !DILocation(line: 1319, column: 13, scope: !276)
!286 = !DILocation(line: 1320, column: 13, scope: !276)
!287 = !DILocation(line: 1321, column: 17, scope: !276)
!288 = !DILocation(line: 1326, column: 15, scope: !289)
!289 = distinct !DILexicalBlock(scope: !290, file: !45, line: 1321, column: 37)
!290 = distinct !DILexicalBlock(scope: !276, file: !45, line: 1321, column: 17)
!291 = !DILocation(line: 1327, column: 13, scope: !289)
!292 = !DILocation(line: 1328, column: 13, scope: !293)
!293 = distinct !DILexicalBlock(scope: !276, file: !45, line: 1328, column: 13)
!294 = !DILocation(line: 1329, column: 24, scope: !295)
!295 = distinct !DILexicalBlock(scope: !296, file: !45, line: 1329, column: 19)
!296 = distinct !DILexicalBlock(scope: !293, file: !45, line: 1328, column: 13)
!297 = !DILocation(line: 1329, column: 31, scope: !295)
!298 = !DILocation(line: 1332, column: 53, scope: !299)
!299 = distinct !DILexicalBlock(scope: !295, file: !45, line: 1331, column: 42)
!300 = !DILocation(line: 1332, column: 17, scope: !299)
!301 = !DILocation(line: 1334, column: 17, scope: !299)
!302 = !DILocation(line: 1335, column: 21, scope: !299)
!303 = !DILocation(line: 1340, column: 19, scope: !304)
!304 = distinct !DILexicalBlock(scope: !305, file: !45, line: 1335, column: 41)
!305 = distinct !DILexicalBlock(scope: !299, file: !45, line: 1335, column: 21)
!306 = !DILocation(line: 1341, column: 17, scope: !304)
!307 = !DILocation(line: 1343, column: 17, scope: !299)
!308 = !DILocalVariable(name: "m_coeff", scope: !309, file: !45, line: 1723, type: !144)
!309 = distinct !DISubprogram(name: "side_link_3f_force", scope: !45, file: !45, line: 1719, type: !310, scopeLine: 1722, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !313)
!310 = !DISubroutineType(types: !311)
!311 = !{null, !33, !33, !312, !23, !23, !23, !23}
!312 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
!313 = !{!314, !315, !316, !317, !318, !319, !320, !308}
!314 = !DILocalVariable(name: "mu", arg: 1, scope: !309, file: !45, line: 1719, type: !33)
!315 = !DILocalVariable(name: "nu", arg: 2, scope: !309, file: !45, line: 1719, type: !33)
!316 = !DILocalVariable(name: "coeff", arg: 3, scope: !309, file: !45, line: 1719, type: !312)
!317 = !DILocalVariable(name: "Path", arg: 4, scope: !309, file: !45, line: 1720, type: !23)
!318 = !DILocalVariable(name: "Path_nu", arg: 5, scope: !309, file: !45, line: 1720, type: !23)
!319 = !DILocalVariable(name: "Path_mu", arg: 6, scope: !309, file: !45, line: 1721, type: !23)
!320 = !DILocalVariable(name: "Path_numu", arg: 7, scope: !309, file: !45, line: 1722, type: !23)
!321 = !DILocation(line: 0, scope: !309, inlinedAt: !322)
!322 = distinct !DILocation(line: 1344, column: 17, scope: !299)
!323 = !DILocation(line: 1723, column: 3, scope: !309, inlinedAt: !322)
!324 = distinct !DIAssignID()
!325 = !DILocation(line: 1725, column: 16, scope: !309, inlinedAt: !322)
!326 = !DILocation(line: 1725, column: 14, scope: !309, inlinedAt: !322)
!327 = !DILocation(line: 1728, column: 7, scope: !328, inlinedAt: !322)
!328 = distinct !DILexicalBlock(scope: !309, file: !45, line: 1728, column: 7)
!329 = !DILocation(line: 1728, column: 7, scope: !309, inlinedAt: !322)
!330 = !DILocation(line: 1734, column: 9, scope: !331, inlinedAt: !322)
!331 = distinct !DILexicalBlock(scope: !328, file: !45, line: 1728, column: 26)
!332 = !DILocation(line: 1735, column: 7, scope: !333, inlinedAt: !322)
!333 = distinct !DILexicalBlock(scope: !331, file: !45, line: 1734, column: 9)
!334 = !DILocation(line: 1737, column: 7, scope: !333, inlinedAt: !322)
!335 = !DILocation(line: 1746, column: 9, scope: !336, inlinedAt: !322)
!336 = distinct !DILexicalBlock(scope: !328, file: !45, line: 1740, column: 3)
!337 = !DILocation(line: 1747, column: 7, scope: !338, inlinedAt: !322)
!338 = distinct !DILexicalBlock(scope: !336, file: !45, line: 1746, column: 9)
!339 = !DILocation(line: 1749, column: 7, scope: !338, inlinedAt: !322)
!340 = !DILocation(line: 1751, column: 1, scope: !309, inlinedAt: !322)
!341 = !DILocation(line: 1347, column: 28, scope: !299)
!342 = !DILocation(line: 1347, column: 39, scope: !299)
!343 = !DILocation(line: 1348, column: 39, scope: !299)
!344 = !DILocation(line: 1349, column: 17, scope: !345)
!345 = distinct !DILexicalBlock(scope: !346, file: !45, line: 1349, column: 17)
!346 = distinct !DILexicalBlock(scope: !299, file: !45, line: 1349, column: 17)
!347 = !DILocation(line: 1349, column: 17, scope: !346)
!348 = !DILocation(line: 1350, column: 48, scope: !349)
!349 = distinct !DILexicalBlock(scope: !345, file: !45, line: 1349, column: 35)
!350 = !DILocation(line: 1350, column: 63, scope: !349)
!351 = !DILocation(line: 1350, column: 19, scope: !349)
!352 = !DILocation(line: 1352, column: 48, scope: !349)
!353 = !DILocation(line: 1352, column: 63, scope: !349)
!354 = !DILocation(line: 1352, column: 19, scope: !349)
!355 = distinct !{!355, !347, !356}
!356 = !DILocation(line: 1354, column: 17, scope: !346)
!357 = !DILocation(line: 1328, column: 39, scope: !296)
!358 = !DILocation(line: 1328, column: 31, scope: !296)
!359 = distinct !{!359, !292, !360}
!360 = !DILocation(line: 1355, column: 15, scope: !293)
!361 = !DILocation(line: 1358, column: 13, scope: !276)
!362 = !DILocation(line: 0, scope: !309, inlinedAt: !363)
!363 = distinct !DILocation(line: 1359, column: 13, scope: !276)
!364 = !DILocation(line: 1723, column: 3, scope: !309, inlinedAt: !363)
!365 = distinct !DIAssignID()
!366 = !DILocation(line: 1725, column: 17, scope: !309, inlinedAt: !363)
!367 = !DILocation(line: 1725, column: 16, scope: !309, inlinedAt: !363)
!368 = !DILocation(line: 1725, column: 14, scope: !309, inlinedAt: !363)
!369 = !DILocation(line: 1728, column: 7, scope: !328, inlinedAt: !363)
!370 = !DILocation(line: 1728, column: 7, scope: !309, inlinedAt: !363)
!371 = !DILocation(line: 1734, column: 9, scope: !331, inlinedAt: !363)
!372 = !DILocation(line: 1735, column: 7, scope: !333, inlinedAt: !363)
!373 = !DILocation(line: 1737, column: 7, scope: !333, inlinedAt: !363)
!374 = !DILocation(line: 1746, column: 9, scope: !336, inlinedAt: !363)
!375 = !DILocation(line: 1747, column: 7, scope: !338, inlinedAt: !363)
!376 = !DILocation(line: 1749, column: 7, scope: !338, inlinedAt: !363)
!377 = !DILocation(line: 1751, column: 1, scope: !309, inlinedAt: !363)
!378 = !DILocation(line: 1363, column: 13, scope: !379)
!379 = distinct !DILexicalBlock(scope: !380, file: !45, line: 1363, column: 13)
!380 = distinct !DILexicalBlock(scope: !276, file: !45, line: 1363, column: 13)
!381 = !DILocation(line: 1363, column: 13, scope: !380)
!382 = !DILocation(line: 1364, column: 44, scope: !383)
!383 = distinct !DILexicalBlock(scope: !379, file: !45, line: 1363, column: 31)
!384 = !DILocation(line: 1364, column: 59, scope: !383)
!385 = !DILocation(line: 1364, column: 15, scope: !383)
!386 = !DILocation(line: 1366, column: 44, scope: !383)
!387 = !DILocation(line: 1366, column: 59, scope: !383)
!388 = !DILocation(line: 1366, column: 15, scope: !383)
!389 = distinct !{!389, !381, !390}
!390 = !DILocation(line: 1368, column: 13, scope: !380)
!391 = !DILocation(line: 1316, column: 32, scope: !278)
!392 = !DILocation(line: 1316, column: 25, scope: !278)
!393 = distinct !{!393, !281, !394}
!394 = !DILocation(line: 1369, column: 11, scope: !279)
!395 = !DILocation(line: 1373, column: 9, scope: !267)
!396 = !DILocation(line: 1374, column: 9, scope: !267)
!397 = !DILocation(line: 1375, column: 13, scope: !267)
!398 = !DILocation(line: 1380, column: 11, scope: !399)
!399 = distinct !DILexicalBlock(scope: !400, file: !45, line: 1375, column: 33)
!400 = distinct !DILexicalBlock(scope: !267, file: !45, line: 1375, column: 13)
!401 = !DILocation(line: 1381, column: 9, scope: !399)
!402 = !DILocation(line: 1383, column: 9, scope: !267)
!403 = !DILocation(line: 0, scope: !309, inlinedAt: !404)
!404 = distinct !DILocation(line: 1384, column: 9, scope: !267)
!405 = !DILocation(line: 1723, column: 3, scope: !309, inlinedAt: !404)
!406 = distinct !DIAssignID()
!407 = !DILocation(line: 1725, column: 17, scope: !309, inlinedAt: !404)
!408 = !DILocation(line: 1725, column: 16, scope: !309, inlinedAt: !404)
!409 = !DILocation(line: 1725, column: 14, scope: !309, inlinedAt: !404)
!410 = !DILocation(line: 1728, column: 7, scope: !328, inlinedAt: !404)
!411 = !DILocation(line: 1728, column: 7, scope: !309, inlinedAt: !404)
!412 = !DILocation(line: 1734, column: 9, scope: !331, inlinedAt: !404)
!413 = !DILocation(line: 1735, column: 7, scope: !333, inlinedAt: !404)
!414 = !DILocation(line: 1737, column: 7, scope: !333, inlinedAt: !404)
!415 = !DILocation(line: 1746, column: 9, scope: !336, inlinedAt: !404)
!416 = !DILocation(line: 1747, column: 7, scope: !338, inlinedAt: !404)
!417 = !DILocation(line: 1749, column: 7, scope: !338, inlinedAt: !404)
!418 = !DILocation(line: 1751, column: 1, scope: !309, inlinedAt: !404)
!419 = !DILocation(line: 1386, column: 20, scope: !267)
!420 = !DILocation(line: 1386, column: 30, scope: !267)
!421 = !DILocation(line: 1387, column: 20, scope: !267)
!422 = !DILocation(line: 1387, column: 30, scope: !267)
!423 = !DILocation(line: 1388, column: 9, scope: !424)
!424 = distinct !DILexicalBlock(scope: !425, file: !45, line: 1388, column: 9)
!425 = distinct !DILexicalBlock(scope: !267, file: !45, line: 1388, column: 9)
!426 = !DILocation(line: 1388, column: 9, scope: !425)
!427 = !DILocation(line: 1389, column: 40, scope: !428)
!428 = distinct !DILexicalBlock(scope: !424, file: !45, line: 1388, column: 27)
!429 = !DILocation(line: 1389, column: 55, scope: !428)
!430 = !DILocation(line: 1389, column: 11, scope: !428)
!431 = !DILocation(line: 1391, column: 40, scope: !428)
!432 = !DILocation(line: 1391, column: 55, scope: !428)
!433 = !DILocation(line: 1391, column: 11, scope: !428)
!434 = distinct !{!434, !426, !435}
!435 = !DILocation(line: 1393, column: 9, scope: !425)
!436 = !DILocation(line: 1397, column: 13, scope: !267)
!437 = !DILocation(line: 1398, column: 11, scope: !438)
!438 = distinct !DILexicalBlock(scope: !267, file: !45, line: 1397, column: 13)
!439 = !DILocation(line: 0, scope: !309, inlinedAt: !440)
!440 = distinct !DILocation(line: 1400, column: 9, scope: !267)
!441 = !DILocation(line: 1723, column: 3, scope: !309, inlinedAt: !440)
!442 = !DILocation(line: 1725, column: 16, scope: !309, inlinedAt: !440)
!443 = distinct !DIAssignID()
!444 = !DILocation(line: 1725, column: 14, scope: !309, inlinedAt: !440)
!445 = !DILocation(line: 1734, column: 9, scope: !331, inlinedAt: !440)
!446 = !DILocation(line: 1746, column: 9, scope: !336, inlinedAt: !440)
!447 = !DILocation(line: 1735, column: 7, scope: !333, inlinedAt: !440)
!448 = !DILocation(line: 1737, column: 7, scope: !333, inlinedAt: !440)
!449 = !DILocation(line: 1747, column: 7, scope: !338, inlinedAt: !440)
!450 = !DILocation(line: 1749, column: 7, scope: !338, inlinedAt: !440)
!451 = !DILocation(line: 1751, column: 1, scope: !309, inlinedAt: !440)
!452 = !DILocation(line: 1404, column: 15, scope: !453)
!453 = distinct !DILexicalBlock(scope: !267, file: !45, line: 1404, column: 13)
!454 = !DILocation(line: 1404, column: 13, scope: !267)
!455 = !DILocation(line: 1405, column: 27, scope: !456)
!456 = distinct !DILexicalBlock(scope: !453, file: !45, line: 1404, column: 33)
!457 = !DILocation(line: 1406, column: 15, scope: !458)
!458 = distinct !DILexicalBlock(scope: !456, file: !45, line: 1406, column: 15)
!459 = !DILocation(line: 1406, column: 15, scope: !456)
!460 = !DILocation(line: 1411, column: 13, scope: !461)
!461 = distinct !DILexicalBlock(scope: !458, file: !45, line: 1407, column: 11)
!462 = !DILocation(line: 1415, column: 13, scope: !461)
!463 = !DILocation(line: 1418, column: 13, scope: !461)
!464 = !DILocation(line: 1420, column: 13, scope: !461)
!465 = !DILocation(line: 1422, column: 13, scope: !461)
!466 = !DILocation(line: 1423, column: 11, scope: !461)
!467 = !DILocation(line: 1425, column: 13, scope: !468)
!468 = distinct !DILexicalBlock(scope: !458, file: !45, line: 1424, column: 11)
!469 = !DILocation(line: 1428, column: 13, scope: !468)
!470 = !DILocation(line: 1305, column: 28, scope: !264)
!471 = !DILocation(line: 1305, column: 21, scope: !264)
!472 = distinct !{!472, !258, !473}
!473 = !DILocation(line: 1431, column: 7, scope: !259)
!474 = !DILocation(line: 1304, column: 29, scope: !261)
!475 = !DILocation(line: 1304, column: 21, scope: !261)
!476 = distinct !{!476, !254, !477}
!477 = !DILocation(line: 1433, column: 3, scope: !255)
!478 = !DILocation(line: 1436, column: 3, scope: !65)
!479 = !DILocation(line: 1438, column: 5, scope: !480)
!480 = distinct !DILexicalBlock(scope: !481, file: !45, line: 1437, column: 3)
!481 = distinct !DILexicalBlock(scope: !65, file: !45, line: 1437, column: 3)
!482 = !DILocation(line: 1445, column: 1, scope: !65)
