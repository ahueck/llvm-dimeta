! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

PROGRAM pointer_alloc_2d_bounds
   IMPLICIT NONE
   INTEGER, POINTER :: buf(:, :)
   ALLOCATE (buf(2:4, 3:5))
END PROGRAM pointer_alloc_2d_bounds

! CHECK: SourceLoc:
! CHECK:   Line:            8
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:   Array:           [ 3, 3 ]
! CHECK-NEXT:   Qualifiers:      [ array, ptr ]
