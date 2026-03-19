! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

MODULE m
   TYPE :: t
      CHARACTER(LEN=:), ALLOCATABLE :: s
   END TYPE
END MODULE

PROGRAM char_member_alloc
   USE m
   IMPLICIT NONE
   TYPE(t) :: x
   ALLOCATE(CHARACTER(LEN=20) :: x%s)
END PROGRAM char_member_alloc

! CHECK: SourceLoc:
! CHECK:   Line:            15
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: character, Extent: 1, Encoding: string }
