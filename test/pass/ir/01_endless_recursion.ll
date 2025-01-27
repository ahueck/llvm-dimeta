; RUN: %apply-verifier %s | %filecheck %s
; CHECK-NOT: Segmentation
; TIMEOUT: 3
; REQUIRES: hasopaque

; `DIRootType::find_type_root` caused infinite recursion as follows:  
; Initially, `find_type(%call5.i.i.i2.i6.i)` is invoked.  
; During execution, the ValuePath root type of `%call5.i.i.i2.i6.i` is determined to be `%call5.i.i.i2.i6.i` itself.  
; If the identified root type is a heap-like call, `find_type_root` recursively invokes `find_type()` on it, i.e., `find_type(%call5.i.i.i2.i6.i)`.  
; This self-referential determination led to infinite recursion in `find_type`.

target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @_Z3foov() personality ptr null {
entry:
  br label %_ZNSt12_Vector_baseIPiSaIS0_EEC2EmRKS1_.exit.i

_ZNSt15__new_allocatorIPiE8allocateEmPKv.exit.i.i.i.i: ; No predecessors!
  %call5.i.i.i2.i6.i = call ptr @_Znwm()
  br label %_ZNSt12_Vector_baseIPiSaIS0_EEC2EmRKS1_.exit.i

_ZNSt12_Vector_baseIPiSaIS0_EEC2EmRKS1_.exit.i:   ; preds = %_ZNSt15__new_allocatorIPiE8allocateEmPKv.exit.i.i.i.i, %entry
  %cond.i.i.i.i = phi ptr [ null, %entry ], [ %call5.i.i.i2.i6.i, %_ZNSt15__new_allocatorIPiE8allocateEmPKv.exit.i.i.i.i ]
  br label %_ZNSt6vectorIPiSaIS0_EEC2ERKS2_.exit

if.then2.i.i.i.i.i.i.i.i.i:                       ; No predecessors!
  store ptr null, ptr %cond.i.i.i.i, align 8
  br label %_ZNSt6vectorIPiSaIS0_EEC2ERKS2_.exit

_ZNSt6vectorIPiSaIS0_EEC2ERKS2_.exit:             ; preds = %if.then2.i.i.i.i.i.i.i.i.i, %_ZNSt12_Vector_baseIPiSaIS0_EEC2EmRKS1_.exit.i
  ret void
}

declare ptr @_Znwm()

; uselistorder directives
uselistorder ptr null, { 1, 2, 3, 4, 0 }
