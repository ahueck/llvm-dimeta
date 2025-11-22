! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

FUNCTION foo(n)
   INTEGER, INTENT(IN) :: n
   INTEGER, ALLOCATABLE :: foo(:)
   INTEGER :: i
   ALLOCATE(foo(n))
   !  DO i = 1, n
   !     foo(i) = i * 10
   !  END DO
END FUNCTION foo

! CHECK:  Line:            10
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT: Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
