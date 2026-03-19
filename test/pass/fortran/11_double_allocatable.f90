! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
! : %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang


program nested_allocation
   implicit none
   type :: Mesh
      integer :: num_nodes
      integer :: num_elements
      real, dimension(:,:), allocatable :: coordinates
      integer, dimension(:,:), allocatable :: connectivity
   end type Mesh

   type(Mesh), allocatable :: my_mesh

   allocate(my_mesh)
   my_mesh%num_nodes = 99
   my_mesh%num_elements = 500

   allocate(my_mesh%coordinates(77, my_mesh%num_nodes))
   allocate(my_mesh%connectivity(4, my_mesh%num_elements))
end program nested_allocation


! CHECK:      SourceLoc:
! CHECK:   Line:            18
! CHECK: Builtin:         false
! CHECK-NEXT: Type:
! CHECK-NEXT:   Compound:
! CHECK-NEXT:     Name:            mesh
! CHECK-NEXT:     Type:            struct
! CHECK-NEXT:     Extent:          152
! CHECK-NEXT:     Sizes:           [ 4, 4, 72, 72 ]
! CHECK-NEXT:     Offsets:         [ 0, 4, 8, 80 ]

! CHECK:      SourceLoc:
! CHECK:   Line:             22
! CHECK-NEXT: Builtin:          true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:      { Name: real, Extent: 4, Encoding: float }
! CHECK-NEXT:   Array:            [ 1 ]
! CHECK-NEXT:   Qualifiers:       [ array ]

! CHECK:      SourceLoc:
! CHECK:   Line:             23
! CHECK-NEXT: Builtin:          true
! CHECK-NEXT: Type:
! CHECK-NEXT:   Fundamental:      { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:   Array:            [ 1 ]
! CHECK-NEXT:   Qualifiers:       [ array ]
