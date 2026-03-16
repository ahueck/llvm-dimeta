! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

SUBROUTINE use_local_char()
   IMPLICIT NONE
   CHARACTER(LEN=32), VOLATILE :: local_stack_char
   local_stack_char = "ABC"
END SUBROUTINE use_local_char

! CHECK: SourceLoc:
! CHECK:   Line:            8
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: character, Extent: 32, Encoding: string }
