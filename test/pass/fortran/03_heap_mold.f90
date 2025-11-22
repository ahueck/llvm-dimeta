! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

subroutine scalar_mold_allocation()
   REAL*8, allocatable :: a(:)
   ! real :: b
   allocate(a(102), mold=2.0D0)
end subroutine

! CHECK:  Line:            9
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! CHECK-NEXT: Array:           [ 102 ]
! CHECK-NEXT: Qualifiers:      [ array ]
