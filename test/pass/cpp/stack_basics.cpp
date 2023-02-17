// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

int foo() {
  bool a{};
  int b{};
  unsigned c{};
  long d{};
  long long int e{};
  unsigned long long int f{};
  short g{};
  unsigned short h{};
  signed char i{};
  bool j{};
  float k{};
  double l{};
  long double m{};
  return b;
}
