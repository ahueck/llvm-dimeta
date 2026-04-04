! RUN: %fortran-to-llvm %s | %apply-verifier | %filecheck %s
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

MODULE test_mod
   IMPLICIT NONE

   TYPE field_type
      REAL(KIND=8), ALLOCATABLE :: density(:,:)
      INTEGER :: x_min, x_max, y_min, y_max
   END TYPE field_type

   TYPE tile_type
      TYPE(field_type):: field

      INTEGER         :: left            &
         ,right           &
         ,bottom          &
         ,top

      INTEGER            :: x_cells              &
         ,y_cells

      INTEGER         :: tile_neighbours(4)
      INTEGER         :: tile_coords(2)

   END TYPE tile_type

   TYPE chunk_type
      INTEGER         :: task   !mpi task
      INTEGER         :: chunk_x_min  &
         ,chunk_y_min  &
         ,chunk_x_max  &
         ,chunk_y_max
      INTEGER            :: x_cells              &
         ,y_cells
      INTEGER         :: left            &
         ,right           &
         ,bottom          &
         ,top
      INTEGER         :: chunk_neighbours(4)
      REAL(KIND=8),ALLOCATABLE:: left_rcv_buffer(:),right_rcv_buffer(:),bottom_rcv_buffer(:),top_rcv_buffer(:)
      REAL(KIND=8),ALLOCATABLE:: left_snd_buffer(:),right_snd_buffer(:),bottom_snd_buffer(:),top_snd_buffer(:)
      TYPE(tile_type), DIMENSION(:), ALLOCATABLE :: tiles
      INTEGER,DIMENSION(2) :: tile_dims
      INTEGER,DIMENSION(2) :: sub_tile_dims
      INTEGER :: halo_exchange_depth
   END TYPE chunk_type

CONTAINS

   SUBROUTINE do_alloc(t)
      INTEGER, INTENT(IN) :: t
      TYPE(chunk_type), ALLOCATABLE :: chunk
      ALLOCATE(chunk)
      ALLOCATE(chunk%tiles(t))
      ! Allocated type: REAL(KIND=8), DIMENSION(:,:)
      ALLOCATE(chunk%tiles(t)%field%density &
         (chunk%tiles(t)%field%x_min-chunk%halo_exchange_depth: &
         chunk%tiles(t)%field%x_max+chunk%halo_exchange_depth, &
         chunk%tiles(t)%field%y_min-chunk%halo_exchange_depth: &
         chunk%tiles(t)%field%y_max+chunk%halo_exchange_depth))
   END SUBROUTINE do_alloc

END MODULE test_mod

! CHECK:      Line:            57
! CHECK-NEXT: Builtin:         false
! CHECK-NEXT: Type:
! CHECK-NEXT:   Compound:
! CHECK-NEXT:     Name:            tile_type
! CHECK-NEXT:     Type:            struct
! CHECK-NEXT:     Extent:          136
! CHECK-NEXT:     Sizes:           [ 88, 4, 4, 4, 4, 4, 4, 16, 8 ]

! CHECK: Line:            59
! CHECK: Fundamental:     { Name: {{real|'real\(kind=8\)'}}, Extent: 8, Encoding: float }
! CHECK: Qualifiers:      [ array ]
