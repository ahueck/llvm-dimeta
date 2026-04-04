! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
!  %fortran-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang



MODULE tea_module
   IMPLICIT NONE
   TYPE :: TileType
      INTEGER :: x_cells = 10
      INTEGER :: y_cells = 10
   END TYPE TileType
   TYPE :: ChunkType
      TYPE(TileType), ALLOCATABLE :: tiles(:)
      INTEGER :: halo_exchange_depth = 0
   END TYPE ChunkType

   ! TYPE(ChunkType) :: chunk
   INTEGER :: tiles_per_task = 3

CONTAINS

   SUBROUTINE build_field()
      IMPLICIT NONE
      TYPE(ChunkType), ALLOCATABLE :: local_chunk
      ALLOCATE(local_chunk)
      ALLOCATE(local_chunk%tiles(tiles_per_task))
   END SUBROUTINE build_field

END MODULE tea_module

! CHECK:     Name:            chunktype
! CHECK-NEXT:     Type:            struct
! CHECK-NEXT:     Extent:          68
! CHECK-NEXT:     Sizes:           [ 64, 4 ]
! CHECK-NEXT:     Offsets:         [ 0, 64 ]
! CHECK-NEXT:     Members:
! CHECK-NEXT:       - Name:            tiles
! CHECK-NEXT:         Builtin:         false
! CHECK-NEXT:         Type:
! CHECK-NEXT:           Compound:
! CHECK-NEXT:             Name:            tiletype
! CHECK-NEXT:             Type:            struct
! CHECK-NEXT:             Extent:          8
! CHECK-NEXT:             Sizes:           [ 4, 4 ]
! CHECK-NEXT:             Offsets:         [ 0, 4 ]
! CHECK-NEXT:             Members:
! CHECK-NEXT:               - Name:            x_cells
! CHECK-NEXT:                 Builtin:         true
! CHECK-NEXT:                 Type:
! CHECK-NEXT:                   Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:               - Name:            y_cells
! CHECK-NEXT:                 Builtin:         true
! CHECK-NEXT:                 Type:
! CHECK-NEXT:                   Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:           Array:           [ 1 ]
! CHECK-NEXT:           Qualifiers:      [ array ]
! CHECK-NEXT:       - Name:            halo_exchange_depth
! CHECK-NEXT:         Builtin:         true
! CHECK-NEXT:         Type:
! CHECK-NEXT:           Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:   Qualifiers:      [ ptr ]

! CHECK:   Line:            28
! CHECK-NEXT: Builtin:         false
! CHECK-NEXT: Type:
! CHECK-NEXT:   Compound:
! CHECK-NEXT:     Name:            tiletype
! CHECK-NEXT:     Type:            struct
! CHECK-NEXT:     Extent:          8
! CHECK-NEXT:     Sizes:           [ 4, 4 ]
! CHECK-NEXT:     Offsets:         [ 0, 4 ]
! CHECK-NEXT:     Members:
! CHECK-NEXT:       - Name:            x_cells
! CHECK-NEXT:         Builtin:         true
! CHECK-NEXT:         Type:
! CHECK-NEXT:           Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:       - Name:            y_cells
! CHECK-NEXT:         Builtin:         true
! CHECK-NEXT:         Type:
! CHECK-NEXT:           Fundamental:     { Name: integer, Extent: 4, Encoding: signed_int }
! CHECK-NEXT:   Array:           [ 3 ]
! CHECK-NEXT:   Qualifiers:      [ array ]
