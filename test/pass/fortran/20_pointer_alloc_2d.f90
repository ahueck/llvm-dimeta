! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

PROGRAM pointer_alloc_2d
   IMPLICIT NONE
   REAL, POINTER :: p_2d(:,:)
   ALLOCATE(p_2d(5, 5))
END PROGRAM pointer_alloc_2d

! CHECK: SourceLoc:
! CHECK:   Line:            8
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: real, Extent: 4, Encoding: float }
! CHECK-NEXT:   Array:           [ 5, 5 ]
! CHECK-NEXT:   Qualifiers:      [ array, ptr ]
