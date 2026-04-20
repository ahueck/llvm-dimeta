! RUN: %fortran-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

! REQUIRES: hasflang

MODULE tea_module
   IMPLICIT NONE

   TYPE :: TileType
      INTEGER :: x_cells = 10
      INTEGER :: y_cells = 10
   END TYPE TileType

   TYPE :: ChunkType
      TYPE(TileType), ALLOCATABLE :: tiles(:, :)
      INTEGER :: halo_exchange_depth = 0
   END TYPE ChunkType

   INTEGER :: tiles_per_task = 3
   INTEGER :: tiles_per_task_y = 2

CONTAINS

   SUBROUTINE build_field()
      IMPLICIT NONE
      TYPE(ChunkType), ALLOCATABLE :: local_chunk
      ALLOCATE(local_chunk)
      ALLOCATE(local_chunk%tiles(tiles_per_task, tiles_per_task_y))
   END SUBROUTINE build_field

END MODULE tea_module

! CHECK:      Line:            27
! CHECK-NEXT: Builtin:         false
! CHECK-NEXT: Type:
! CHECK-NEXT:   Compound:
! CHECK-NEXT:     Name:            tiletype
! CHECK:          Array:           [ 3, 2 ]
! CHECK:          Qualifiers:      [ array ]
