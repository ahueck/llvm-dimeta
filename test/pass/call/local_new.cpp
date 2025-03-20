// RUN: %cpp-to-llvm %s | %apply-verifier -kernel-call 2>&1 | %filecheck %s

struct Domain {
  float stoptime;
  float time;
  char c;
};

void kernel(float, Domain*);

void test() {
  auto* d = new Domain;
  // CHECK: Final value type:
  // CHECK: Fundamental:     { Name: float, Extent: 4, Encoding: float }

  // CHECK: Final value type:
  // CHECK: Compound:
  // CHECK-NEXT:   Name:            Domain
  // CHECK-NEXT:   Identifier:      _ZTS6Domain
  // CHECK-NEXT:   Type:            struct
  // CHECK-NEXT:   Extent:          12
  kernel(d->time, d);
}
