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

   type(ExtType), allocatable :: test_obj
   allocate(test_obj)
   test_obj%id = 1
   test_obj%extra_value = 3.14

   ! deallocate(test_obj)
end program test_polymorphic_alloc

! CHECK: SourceLoc:
! CHECK:   Line:            16
! CHECK-NEXT: Builtin:         false
! CHECK-NEXT: Type:
! CHECK-NEXT:   Compound:
! CHECK-NEXT:     Name:            exttype
! CHECK-NEXT:     Type:            struct
! CHECK-NEXT:     Extent:          8
! CHECK-NEXT:     Sizes:           [ 4, 4 ]
! CHECK-NEXT:     Offsets:         [ 0, 4 ]
! CHECK-NEXT:     Members:
! CHECK-NEXT:       - Name:            basetype
! CHECK-NEXT:         Builtin:         false
! CHECK-NEXT:         Type:
! CHECK-NEXT:           Compound:
! CHECK-NEXT:             Name:            basetype
! CHECK-NEXT:             Type:            struct
! CHECK-NEXT:             Extent:          4
! CHECK-NEXT:             Sizes:           [ 4 ]
! CHECK-NEXT:             Offsets:         [ 0 ]
! CHECK-NEXT:             Members:
! CHECK-NEXT:               - Name:            id
! CHECK-NEXT:                 Builtin:         true
! CHECK-NEXT:                 Type:
! CHECK-NEXT:                   Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:       - Name:            extra_value
! CHECK-NEXT:         Builtin:         true
! CHECK-NEXT:         Type:
! CHECK-NEXT:           Fundamental:     { Name: real, Extent: 4, Encoding: float }
! CHECK-NEXT:  Qualifiers:      [ ptr ]
