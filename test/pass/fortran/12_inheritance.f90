! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

program test_polymorphic_alloc
   implicit none
   type :: BaseType
      integer :: id = 0
   end type BaseType
   type, extends(BaseType) :: ExtType
      real :: extra_value = 0.0
   end type ExtType

   class(BaseType), allocatable :: test_obj
   allocate(ExtType :: test_obj)
   ! deallocate(test_obj)
end program test_polymorphic_alloc

! Caveat: no debug data for ExtType, only internal descriptor

! CHECK: SourceLoc:
! CHECK:   Line:            16
! CHECK-NEXT: Builtin:         false
! CHECK-NEXT: Type:
! CHECK-NEXT:   Compound:
! CHECK-NEXT:     Name:            basetype
! CHECK-NEXT:     Type:            struct
! CHECK-NEXT:     Extent:          4
! CHECK-NEXT:     Sizes:           [ 4 ]
! CHECK-NEXT:     Offsets:         [ 0 ]
! CHECK-NEXT:     Members:
! CHECK-NEXT:       - Name:            id
! CHECK-NEXT:         Builtin:         true
! CHECK-NEXT:         Type:
! CHECK-NEXT:           Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:   Qualifiers:      [ ptr ]
