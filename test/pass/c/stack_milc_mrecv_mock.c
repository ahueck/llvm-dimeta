// RUN: %c-to-llvm %s | %apply-verifier -yaml 2>&1 | %filecheck %s

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

void foo() {
  msg_sr_t mrecv;
}

// CHECK: SourceLoc:
// CHECK-NEXT:   File:            {{'?}}{{.*}}stack_milc_mrecv_mock.c{{'?}}
// CHECK-NEXT:   Function:        foo
// CHECK-NEXT:   Line:            21
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            ''
// CHECK-NEXT:     Type:            struct
// CHECK-NEXT:     Extent:          32
// CHECK-NEXT:     Sizes:           [ 4, 4, 4, 8, 8 ]
// CHECK-NEXT:     Offsets:         [ 0, 4, 8, 16, 24 ]
// CHECK-NEXT:     Members:
// CHECK-NEXT:       - Name:            msg_node
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:       - Name:            id_offset
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:       - Name:            msg_size
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:       - Name:            msg_buf
// CHECK-NEXT:         Builtin:         true
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Fundamental:     { Name: char, Extent: 8, Encoding: signed_char }
// CHECK-NEXT:           Qualifiers:      [ ptr ]
// CHECK-NEXT:       - Name:            gmem
// CHECK-NEXT:         Builtin:         false
// CHECK-NEXT:         Type:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            gmem_t
// CHECK-NEXT:             Type:            struct
// CHECK-NEXT:             Extent:          40
// CHECK-NEXT:             Sizes:           [ 8, 4, 4, 4, 8, 8 ]
// CHECK-NEXT:             Offsets:         [ 0, 8, 12, 16, 24, 32 ]
// CHECK-NEXT:             Members:
// CHECK-NEXT:               - Name:            mem
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: char, Extent: 8, Encoding: signed_char }
// CHECK-NEXT:                   Qualifiers:      [ ptr ]
// CHECK-NEXT:               - Name:            size
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:               - Name:            stride
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:               - Name:            num
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:               - Name:            sitelist
// CHECK-NEXT:                 Builtin:         true
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Fundamental:     { Name: int, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:                   Qualifiers:      [ ptr ]
// CHECK-NEXT:               - Name:            next
// CHECK-NEXT:                 Builtin:         false
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Compound:
// CHECK-NEXT:                     Name:            gmem_t
// CHECK-NEXT:                     Type:            struct
// CHECK-NEXT:                     Extent:          40
// CHECK-NEXT:                   Qualifiers:      [ ptr ]
// CHECK-NEXT:                   Recurring:       true
// CHECK-NEXT:           Qualifiers:      [ ptr ]
// CHECK-NEXT:           Typedef:         gmem_t
// CHECK-NEXT:   Typedef:         msg_sr_t