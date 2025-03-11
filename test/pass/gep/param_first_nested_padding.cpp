// the code becomes a store of malloc to chunky (no gep etc.) -> need to rely on TBAA:
// RUN: %cpp-to-llvm %s | %opt -O1 -S | %apply-verifier 2>&1 | %filecheck %s

extern "C" {
void* malloc(unsigned);
}

struct foo {
  struct {
    struct xx {
      int padding;
      struct bb {
        double* get_x() {
          return x;
        }
        double* get_z() {
          return x;
        }
        double* x;
        float* z;

      } baz;
    } foobar;
  } bar;
};

void take_field(foo* chunky2) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
  chunky2->bar.foobar.baz.x = (double*)malloc(sizeof(double));
}