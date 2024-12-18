// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// RUN: %c-to-llvm %s | %opt -O2 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

typedef struct {
  float x; /* X coordinate value */
  float y; /* Y coordinate value */
  float z; /* Z coordinate value */
} vector;

#define RT_OBJECT_HEAD                                          \
  unsigned int id;         /* Unique Object serial number    */ \
  void* nextobj;           /* pointer to next object in list */ \
  object_methods* methods; /* this object's methods          */ \
  clip_group* clip;        /* this object's clip group       */ \
  texture* tex;            /* object texture                 */

typedef struct {
  int x;
} object;

typedef struct objectlist {
  struct objectlist* next; /* next link in the list */
  object* obj;             /* the actual object     */
} objectlist;

typedef struct {
  int xsize;          /* number of cells along the X direction */
  int ysize;          /* number of cells along the Y direction */
  int zsize;          /* number of cells along the Z direction */
  vector min;         /* the minimum coords for the box containing the grid */
  vector max;         /* the maximum coords for the box containing the grid */
  vector voxsize;     /* the size of a grid cell/voxel */
  object* objects;    /* all objects contained in the grid */
  objectlist** cells; /* the grid cells themselves */
} grid;

typedef struct {
  int x; /* Voxel X address */
  int y; /* Voxel Y address */
  int z; /* Voxel Z address */
} gridindex;

void newgrid(int numcells, grid* g) {
  g->cells = (objectlist**)malloc(numcells * sizeof(objectlist*));
}
// clang-format off
// CHECK: Final Type: !{{[0-9]+}} = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "objectlist"
// CHECK-NEXT: Pointer level: 2