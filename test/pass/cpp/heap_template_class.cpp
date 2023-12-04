// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

// CHECK: Final Type: {{.*}} = {{distinct}} !DICompositeType(tag: DW_TAG_structure_type, name: "Data<double>"
// CHECK-NEXT: Pointer level: 1

template <typename DType>
struct Data {
  DType data;
};
using doubletype = double;

Data<doubletype>* foo(int n) {
  return new Data<doubletype>;
}
