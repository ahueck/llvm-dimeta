// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 | %filecheck %s

#include <cstddef>
#include <cstdlib>

typedef float Real_t;
typedef int Index_t;

template <typename T>
T* Allocate(std::size_t size) {
  return static_cast<T*>(malloc(sizeof(T) * size));
}

void foo(Real_t** v, Index_t** i, int numElem8) {
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "float", size: 32
  // CHECK-NEXT: Pointer level: 1
  *v = Allocate<Real_t>(numElem8);
  // CHECK: Final Type: {{.*}} = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
  // CHECK-NEXT: Pointer level: 1
  *i = Allocate<Index_t>(numElem8 * 2);
}