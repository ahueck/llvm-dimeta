// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// REQUIRES: heapallocsite

// CHECK: Final Type: {{.*}} = {{distinct}} !DICompositeType(tag: DW_TAG_structure_type, name: "BaseClass"
// CHECK-NEXT: Pointer level: 1

// CHECK: Final Type: {{.*}} = {{distinct}} !DICompositeType(tag: DW_TAG_structure_type, name: "A"
// CHECK-NEXT: Pointer level: 1

// CHECK: Final Type: {{.*}} = {{distinct}} !DICompositeType(tag: DW_TAG_structure_type, name: "B"
// CHECK-NEXT: Pointer level: 1

struct BaseClass {
  int a;
};

struct A : public BaseClass {
  double d;
};

struct B : public BaseClass {
  float f;
};

BaseClass* foo(int n) {
  switch (n) {
    default:
      return new BaseClass;
    case 2:
      return new A;
    case 3:
      return new B;
  }
}
