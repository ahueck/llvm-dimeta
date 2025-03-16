// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Yaml Verifier: 1

extern "C" {
void* malloc(int);
}

struct Base {};

struct Derived : public Base {
  int* c;
};

void foo(Derived* d) {
  d->c = (int*)malloc(sizeof(int));
}

// CHECK: Type for heap-like:{{.*}}call {{.*}}@malloc(
// CHECK-NEXT: Extracted Type: !{{.*}} = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !{{.*}}, size: 64)
// CHECK-NEXT: Final Type: !{{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
// CHECK-NEXT: Pointer level: 1 (T*)
