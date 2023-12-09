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
  // CHECK: Pointer level: 1
  mip         = (mipmap*)malloc(sizeof(mipmap));
  mip->levels = 2;
  // CHECK: Pointer level: 2
  mip->images = (rawimage**)malloc(mip->levels * sizeof(rawimage*));
  return mip;
}
