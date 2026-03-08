! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s --check-prefixes=CHECK,NONOPT
! RUN: %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s --check-prefixes=CHECK
! REQUIRES: hasflang

MODULE test_mod
   IMPLICIT NONE

   TYPE field_type
      REAL(KIND=8), ALLOCATABLE :: energy0(:,:), energy1(:,:), u(:,:)
      REAL(KIND=8), ALLOCATABLE :: cellx(:), celly(:), vertexx(:), vertexy(:)
      INTEGER :: x_min, x_max, y_min, y_max
   END TYPE field_type

   TYPE tile_type
      TYPE(field_type):: field
   END TYPE tile_type

   TYPE chunk_type
      TYPE(tile_type), DIMENSION(:), ALLOCATABLE :: tiles
      INTEGER :: halo_exchange_depth
   END TYPE chunk_type

CONTAINS
   SUBROUTINE do_alloc(t, chunk)
      INTEGER, INTENT(IN) :: t
      TYPE(chunk_type), INTENT(INOUT) :: chunk

      ! ALLOCATE(chunk%tiles(t))
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

! CHECK: Line:            29
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            35
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            41
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            47
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            48
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            49
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]

! CHECK: Line:            50
! CHECK: Fundamental:     { Name: real, Extent: 8, Encoding: float }
! NONOPT: Qualifiers:      [ array ]
