! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

SUBROUTINE use_local_array()
   IMPLICIT NONE
   INTEGER, PARAMETER :: SIZE = 7
   INTEGER :: local_stack_array(SIZE)
   INTEGER :: i
   DO i = 1, SIZE
      local_stack_array(i) = i * 10
   END DO
END SUBROUTINE use_local_array

! CHECK:  Line:            8
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT: Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT: Array:           [ 7 ]
! CHECK-NEXT: Qualifiers:      [ array ]
