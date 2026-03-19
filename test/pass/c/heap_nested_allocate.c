// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

struct field_type {
  double* density;
  int x_min, x_max, y_min, y_max;
};

typedef struct field_type field_type;

struct tile_type {
  field_type field;
  int left, right, bottom, top;
  int x_cells, y_cells;
  int tile_neighbours[4];
  int tile_coords[2];
};

typedef struct tile_type tile_type;

struct chunk_type {
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
};

typedef struct chunk_type chunk_type;

void do_alloc(int t) {
  chunk_type* chunk = (chunk_type*)malloc(sizeof(chunk_type));

  // CHECK:      Line:            47
  // CHECK-NEXT: Builtin:         false
  // CHECK-NEXT: Type:
  // CHECK-NEXT:   Compound:
  // CHECK-NEXT:     Name:            tile_type
  // CHECK-NEXT:     Type:            struct
  chunk->tiles = (tile_type*)malloc(t * sizeof(tile_type));

  // CHECK:      Line:            52
  // CHECK:      Fundamental:     { Name: double, Extent: 8, Encoding: float }
  // CHECK:      Qualifiers:      [ ptr ]
  chunk->tiles[0].field.density = (double*)malloc(10 * sizeof(double));
}
