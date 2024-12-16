// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O2 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

typedef struct {
  int loaded;
  int xres;
  int yres;
  int bpp;
  char name[96];
  unsigned char* data;
} rawimage;

typedef struct {
  int levels;
  rawimage** images;
} mipmap;

mipmap* CreateMIPMap() {
  mipmap* mip;
  // CHECK: Name:            ''
  // CHECK-NEXT: Type:            struct
  // CHECK-NEXT: Extent:          16
  // CHECK-NEXT: Sizes:           [ 4, 8 ]
  // CHECK-NEXT: Offsets:         [ 0, 8 ]
  mip         = (mipmap*)malloc(sizeof(mipmap));
  mip->levels = 2;
  // CHECK: Name:            ''
  // CHECK-NEXT: Type:            struct
  // CHECK-NEXT: Extent:          120
  // CHECK-NEXT: Sizes:           [ 4, 4, 4, 4, 96, 8 ]
  // CHECK-NEXT: Offsets:         [ 0, 4, 8, 12, 16, 112 ]
  mip->images = (rawimage**)malloc(mip->levels * sizeof(rawimage*));
  return mip;
}
