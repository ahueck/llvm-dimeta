// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O2 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char* name;
  void* tex;
} texentry;

typedef struct {
  FILE* ifp;
  const char* filename;
  texentry* textable;
  texentry defaulttex;
  int numtextures;
  int maxtextures;
  int numobjectsparsed;
  long texhash;
} parsehandle;

void add_texture(parsehandle* ph, void* tex, const char* name) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
  ph->textable[ph->numtextures].name = malloc(strlen(name) + 1);
}
