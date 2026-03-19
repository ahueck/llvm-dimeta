! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

FUNCTION foo(n)
   INTEGER, PARAMETER :: DP = KIND(1.0D0)
   INTEGER, INTENT(IN) :: n
   REAL(KIND=DP), ALLOCATABLE :: foo(:)
   INTEGER :: i
   ALLOCATE(foo(n))
   DO i = 1, n
      foo(i) = DBLE(i) * 10.0D0
   END DO
END FUNCTION foo

! CHECK:  Line:            11
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT: Fundamental:     { Name: real, Extent: 8, Encoding: float }
