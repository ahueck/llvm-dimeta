// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O2 | %apply-verifier 2>&1 | %filecheck %s
// From Fortran test 17
#include <stdlib.h>

typedef struct {
  double *energy0, *energy1, *u;
  double *cellx, *celly, *vertexx, *vertexy;
  int x_min, x_max, y_min, y_max;
} field_type;

typedef struct {
  field_type field;
} tile_type;

typedef struct {
  tile_type* tiles;
  int halo_exchange_depth;
} chunk_type;

void do_alloc(int t, chunk_type* chunk) {
  chunk->tiles[t].field.energy0 = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }

  chunk->tiles[t].field.energy1 = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }

  chunk->tiles[t].field.u = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }

  chunk->tiles[t].field.cellx = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }

  chunk->tiles[t].field.celly = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }

  chunk->tiles[t].field.vertexx = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }

  chunk->tiles[t].field.vertexy = (double*)malloc(sizeof(double) * 10);
  // CHECK: Line:            [[@LINE-1]]
  // CHECK: Fundamental:     { Name: double, Extent: 8, Encoding: float }
}
