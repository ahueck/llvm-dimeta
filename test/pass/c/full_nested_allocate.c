// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s
// From Fortran test 18
#include <stdlib.h>

typedef struct {
  float *energy0, *energy1, *u;
  double *cellx, *celly, *vertexx, *vertexy;
  int x_min, x_max, y_min, y_max;
} field_type;

typedef struct {
  field_type field;
  int left, right, bottom, top;
  int x_cells, y_cells;
  int tile_neighbours[4];
  int tile_coords[2];
} tile_type;

typedef struct {
  int task;
  int chunk_x_min, chunk_y_min, chunk_x_max, chunk_y_max;
  int x_cells, y_cells;
  int left, right, bottom, top;
  int chunk_neighbours[4];
  double *left_rcv_buffer, *right_rcv_buffer, *bottom_rcv_buffer, *top_rcv_buffer;
  double *left_snd_buffer, *right_snd_buffer, *bottom_snd_buffer, *top_snd_buffer;
  tile_type* tiles;
  int tile_dims[2];
  int sub_tile_dims[2];
  int halo_exchange_depth;
} chunk_type;

void do_alloc(int t_idx, chunk_type* chunk) {
  // Allocate the tiles array if not already allocated
  if (!chunk->tiles) {
    chunk->tiles = (tile_type*)malloc(sizeof(tile_type) * (t_idx + 1));  // Ensure size for t_idx
  }
  // Initialize some values for bounds, similar to Fortran test
  chunk->halo_exchange_depth      = 1;
  chunk->tiles[t_idx].field.x_min = 0;
  chunk->tiles[t_idx].field.x_max = 9;
  chunk->tiles[t_idx].field.y_min = 0;
  chunk->tiles[t_idx].field.y_max = 9;

  // ALLOCATE(chunk%tiles(t)%field%energy0 ...)
  chunk->tiles[t_idx].field.energy0 = (float*)malloc(sizeof(float) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: float, Extent: 4, Encoding: float }

  // ALLOCATE(chunk%tiles(t)%field%energy1 ...)
  chunk->tiles[t_idx].field.energy1 = (float*)malloc(sizeof(float) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: float, Extent: 4, Encoding: float }

  // ALLOCATE(chunk%tiles(t)%field%u ...)
  chunk->tiles[t_idx].field.u = (float*)malloc(sizeof(float) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: float, Extent: 4, Encoding: float }

  // ALLOCATE(chunk%tiles(t)%field%cellx ...)
  chunk->tiles[t_idx].field.cellx = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }

  // ALLOCATE(chunk%tiles(t)%field%celly ...)
  chunk->tiles[t_idx].field.celly = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }

  // ALLOCATE(chunk%tiles(t)%field%vertexx ...)
  chunk->tiles[t_idx].field.vertexx = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }

  // ALLOCATE(chunk%tiles(t)%field%vertexy ...)
  chunk->tiles[t_idx].field.vertexy = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }
}
