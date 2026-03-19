! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

MODULE m_fixed
   TYPE :: t_fixed
      CHARACTER(LEN=20), ALLOCATABLE :: s_fixed
   END TYPE
END MODULE

SUBROUTINE char_member_fixed_alloc(x)
   USE m_fixed
   IMPLICIT NONE
   TYPE(t_fixed), INTENT(INOUT) :: x
   ALLOCATE(x%s_fixed)
END SUBROUTINE char_member_fixed_alloc

! CHECK: SourceLoc:
! CHECK:   Line:            16
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: character, Extent: 1, Encoding: string }
