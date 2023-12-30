// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s
// RUN: %c-to-llvm %s | %opt -O2 -S | %apply-verifier 2>&1 | %filecheck %s

#include <stdlib.h>

#define NUM_SUBL 2

typedef struct comlink {
  struct comlink* nextcomlink;
  int othernode;
  int n_subl_connected[NUM_SUBL + 1];
  int* sitelist[NUM_SUBL + 1];
} comlink;

typedef struct id_list_t {
  int id_offset;
  struct id_list_t* next;
} id_list_t;

typedef struct gather_t {
  int* neighbor;
  comlink* neighborlist;
  comlink* neighborlist_send;
  id_list_t* id_list;
  int n_recv_msgs, n_send_msgs;
  int offset_increment;
} gather_t;

gather_t* gather_array;

void foo(int sites_on_node, int dir) {
  // CHECK: Extracted Type: {{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: [[DIREF:![0-9]+]], size: 64)
  // CHECK: Final Type: [[DIREF]] = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK-NEXT: Pointer level: 1
  // CHECK: Location: "{{.*}}":"foo":36
  gather_array[dir].neighbor = (int*)malloc(sites_on_node * sizeof(int));
}
