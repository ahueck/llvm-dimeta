! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s --check-prefixes=CHECK,NONOPT
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s --check-prefixes=CHECK
! REQUIRES: hasflang

MODULE test_mod
   IMPLICIT NONE

   TYPE field_type
      REAL(KIND=4), ALLOCATABLE :: energy0(:,:), energy1(:,:), u(:,:)
      REAL(KIND=8), ALLOCATABLE :: cellx(:), celly(:), vertexx(:), vertexy(:)
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

   SUBROUTINE do_alloc(t, chunk)
      INTEGER, INTENT(IN) :: t
      TYPE(chunk_type), INTENT(INOUT) :: chunk

      ALLOCATE(chunk%tiles(t)%field%energy0 &
         (chunk%tiles(t)%field%x_min-chunk%halo_exchange_depth: &
         chunk%tiles(t)%field%x_max+chunk%halo_exchange_depth, &
         chunk%tiles(t)%field%y_min-chunk%halo_exchange_depth: &
         chunk%tiles(t)%field%y_max+chunk%halo_exchange_depth))

      ALLOCATE(chunk%tiles(t)%field%energy1 &
         (chunk%tiles(t)%field%x_min-chunk%halo_exchange_depth: &
         chunk%tiles(t)%field%x_max+chunk%halo_exchange_depth, &
         chunk%tiles(t)%field%y_min-chunk%halo_exchange_depth: &
         chunk%tiles(t)%field%y_max+chunk%halo_exchange_depth))

      ALLOCATE(chunk%tiles(t)%field%u       &
         (chunk%tiles(t)%field%x_min-chunk%halo_exchange_depth: &
         chunk%tiles(t)%field%x_max+chunk%halo_exchange_depth, &
         chunk%tiles(t)%field%y_min-chunk%halo_exchange_depth: &
         chunk%tiles(t)%field%y_max+chunk%halo_exchange_depth))

      ALLOCATE(chunk%tiles(t)%field%cellx   (chunk%tiles(t)%field%x_min-2:chunk%tiles(t)%field%x_max+2))
      ALLOCATE(chunk%tiles(t)%field%celly   (chunk%tiles(t)%field%y_min-2:chunk%tiles(t)%field%y_max+2))
      ALLOCATE(chunk%tiles(t)%field%vertexx (chunk%tiles(t)%field%x_min-2:chunk%tiles(t)%field%x_max+3))
      ALLOCATE(chunk%tiles(t)%field%vertexy (chunk%tiles(t)%field%y_min-2:chunk%tiles(t)%field%y_max+3))

   END SUBROUTINE do_alloc

END MODULE test_mod

! CHECK: Line:            57
! CHECK: Fundamental:     { Name: real, Extent: 4, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            63
! CHECK: Fundamental:     { Name: real, Extent: 4, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            69
! CHECK: Fundamental:     { Name: real, Extent: 4, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            75
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            76
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            77
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            78
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]
