! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

SUBROUTINE print_string(s)
   IMPLICIT NONE
   CHARACTER(LEN=20), INTENT(IN) :: s
   PRINT *, s
END SUBROUTINE print_string

SUBROUTINE test_fixed_char()
   IMPLICIT NONE
   CHARACTER(LEN=20) :: local_char
   local_char = "Hello World"
   CALL print_string(local_char)
END SUBROUTINE test_fixed_char

! CHECK: SourceLoc:
! CHECK:   Line:            14
! CHECK-NEXT: Builtin:         true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:     { Name: character, Extent: 20, Encoding: string }