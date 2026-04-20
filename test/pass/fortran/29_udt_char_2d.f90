! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

! XFAIL: *

MODULE m_udt
   TYPE :: t_udt
      INTEGER :: id
      CHARACTER(LEN=10), ALLOCATABLE :: names(:, :)
   END TYPE t_udt
END MODULE m_udt

SUBROUTINE init_udt(x, s)
   USE m_udt
   IMPLICIT NONE
   TYPE(t_udt), INTENT(INOUT) :: x
   CHARACTER(LEN=1), INTENT(IN) :: s
   IF (.NOT. ALLOCATED(x%names)) THEN
      ALLOCATE(x%names(5, 5))
   END IF
   x%id = 1
END SUBROUTINE init_udt

! CHECK: SourceLoc:
! CHECK:   Line:            16
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }

! CHECK: SourceLoc:
! CHECK:   Line:            16
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: character, Extent: 1, Encoding: string }
! CHECK-NEXT:   Array:           [ -1, -1 ]
! CHECK-NEXT:   Qualifiers:      [ array, allocatable ]
