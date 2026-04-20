! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

PROGRAM pointer_alloc_1d_lower_bound
   IMPLICIT NONE
   INTEGER, POINTER :: buf(:)
   ALLOCATE (buf(0:(10) - 1))
END PROGRAM pointer_alloc_1d_lower_bound

! CHECK: SourceLoc:
! CHECK:   Line:            8
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:   Array:           [ 10 ]
! CHECK-NEXT:   Qualifiers:      [ array, ptr ]
