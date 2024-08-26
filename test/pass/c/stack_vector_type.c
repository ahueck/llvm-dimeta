// RUN: %c-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

typedef float float2 __attribute__((ext_vector_type(2)));

void test2() {
  float xf[2] = {0.f};
}

// CHECK: Fundamental:     { Name: float, Extent: 4, Encoding: float }
// CHECK-NEXT: Array:           2

void test() {
  float2 vf = (float2){1.0f, 2.0f};
}

// CHECK: Fundamental:     { Name: float, Extent: 4, Encoding: float }
// CHECK-NEXT: Array:           2
// CHECK-NEXT: Typedef:         float2
// CHECK-NEXT: Vector:          true
