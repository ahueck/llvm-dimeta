! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

! XFAIL: *

SUBROUTINE char_indexing(s)
   IMPLICIT NONE
   CHARACTER(LEN=10), INTENT(INOUT) :: s
   ! Indexing into a fixed-length string dummy argument
   s(2:2) = 'B'
END SUBROUTINE char_indexing

! CHECK: SourceLoc:
! CHECK:   Line:            8
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: character, Extent: 10, Encoding: string }

SUBROUTINE char_local_indexing()
   IMPLICIT NONE
   CHARACTER(LEN=10) :: s
   s = "1234567890"
   s(2:2) = 'B'
END SUBROUTINE char_local_indexing

! CHECK: SourceLoc:
! CHECK:   Line:            21
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: character, Extent: 10, Encoding: string }

SUBROUTINE char_array_access(s)
   IMPLICIT NONE
   CHARACTER(LEN=10), INTENT(INOUT) :: s(5)
   s(2)(2:2) = 'X'
END SUBROUTINE char_array_access

! CHECK: SourceLoc:
! CHECK:   Line:            34
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: character, Extent: 10, Encoding: string }
! CHECK-NEXT:   Array:           [ 5 ]
