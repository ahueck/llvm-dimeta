! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

SUBROUTINE do_char_alloc(s)
   IMPLICIT NONE
   CHARACTER(LEN=:), ALLOCATABLE, INTENT(INOUT) :: s
   ALLOCATE(CHARACTER(LEN=15) :: s)
END SUBROUTINE do_char_alloc

! CHECK: SourceLoc:
! CHECK:   Line:            9
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: character, Extent: 1, Encoding: string }
