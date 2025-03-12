; RUN: %apply-verifier %s |& %filecheck %s
; CHECK-NOT: Assertion
; REQUIRES: llvm-18 || llvm-19

target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define ptr @_ZNK3MPI8Cartcomm5CloneEv() personality ptr null {
entry:
  %call2 = call ptr @_Znwm()
  ret ptr %call2
}

declare ptr @_Znwm()

; uselistorder directives
uselistorder ptr null, { 1, 2, 0 }