! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

FUNCTION foo(n) result(i)
   INTEGER, INTENT(IN) :: n
   INTEGER, ALLOCATABLE :: foo_a(:)
   INTEGER :: i
   ALLOCATE(foo_a(n))
END FUNCTION foo

! CHECK: SourceLoc:
! CHECK:  Line:            9
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT: Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
