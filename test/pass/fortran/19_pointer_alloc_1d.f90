! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

PROGRAM pointer_alloc_1d
   IMPLICIT NONE
   INTEGER, POINTER :: p_1d(:)
   ALLOCATE(p_1d(10))
END PROGRAM pointer_alloc_1d

! CHECK: SourceLoc:
! CHECK:   Line:            8
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:   Array:           [ 10 ]
! CHECK-NEXT:   Qualifiers:      [ array, ptr ]
