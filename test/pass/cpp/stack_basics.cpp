// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "bool"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "int"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "unsigned int"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "long{{( int)?}}"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "long long{{( int)?}}"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "{{(unsigned long long)|(long long unsigned int)}}"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "short"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "unsigned short"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "signed char"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "bool"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "float"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "double"
// CHECK: Final Type Stack: {{.*}} = !DIBasicType(name: "long double"

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
