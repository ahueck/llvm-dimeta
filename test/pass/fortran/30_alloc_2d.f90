! RUN: %fortran-to-llvm %s | %apply-verifier

! REQUIRES: hasflang

PROGRAM alloc_2d
   IMPLICIT NONE
   INTEGER, ALLOCATABLE :: buf(:, :)
   ALLOCATE(buf(2, 2))
END PROGRAM alloc_2d

 ! CHECK: SourceLoc:
 ! CHECK:   Line:            8
 ! CHECK-NEXT: Builtin:         true
 ! CHECK-NEXT: Type:
 ! CHECK-NEXT:   Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
