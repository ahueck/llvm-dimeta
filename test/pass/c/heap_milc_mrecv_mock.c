// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O2 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

typedef struct gmem_t {
  char* mem;           /* source (destination) address for send (receive) */
  int size;            /* size of sent field */
  int stride;          /* stride of source/destination field */
  int num;             /* number of sites in sitelist */
  int* sitelist;       /* sites gathered to/from */
  struct gmem_t* next; /* linked list */
} gmem_t;

typedef struct {
  int msg_node;  /* node sending or receiving message */
  int id_offset; /* id offset for this message */
  int msg_size;  /* size of message in bytes */
  char* msg_buf; /* address of buffer malloc'd for message */
  gmem_t* gmem;  /* linked list explaining detailed usage for buffer */
} msg_sr_t;

msg_sr_t* mrecv;

void foo(int nsites, int tsize, int i) {
  // CHECK: Final Type: !{{[0-9]+}} = !DIBasicType(name: "char"
  mrecv[i].msg_buf = (char*)malloc(nsites * tsize);
}
