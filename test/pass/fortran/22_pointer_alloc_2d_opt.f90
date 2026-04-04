! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

SUBROUTINE do_alloc_2d(p_2d)
   IMPLICIT NONE
   REAL, POINTER, INTENT(INOUT) :: p_2d(:,:)
   ALLOCATE(p_2d(5, 5))
END SUBROUTINE do_alloc_2d

! CHECK: SourceLoc:
! CHECK:   Line:            8
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: real, Extent: 4, Encoding: float }
! CHECK-NEXT:   Array:           [ 5, 5 ]
! CHECK-NEXT:   Qualifiers:      [ array, ptr ]
