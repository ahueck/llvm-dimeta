// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 -yaml | %filecheck %s

// clang-format off
// CHECK: Name:{{ *}}_Vector_impl
// CHECK-NEXT: Identifier:{{ *}}_ZTSNSt12_Vector_baseIiN19allocator_namespace12my_allocatorIiEEE12_Vector_implE
// CHECK-NEXT: Type:{{ *}}struct
// CHECK-NEXT: Extent:{{ *}}32
// CHECK-NEXT: Sizes:{{ *}}[ 8, 24 ]
// CHECK-NEXT: Offsets:{{ *}}[ 0, 8 ]
// clang-format on

// CHECK: Yaml Verifier: 1

#include <cstddef>
#include <vector>

namespace allocator_namespace {

template <class T>
class my_allocator {
 public:
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T value_type;

  void* padding{nullptr};

  my_allocator() {
  }
  my_allocator(const my_allocator&) {
  }

  pointer allocate(size_type n, const void* = 0) {
    // T* t = (T*)malloc(n * sizeof(T));
    // return t;
    return nullptr;
  }

  void deallocate(void* p, size_type) {
    if (p) {
      //      free(p);
    }
  }

  pointer address(reference x) const {
    return &x;
  }
  const_pointer address(const_reference x) const {
    return &x;
  }
  my_allocator<T>& operator=(const my_allocator&) {
    return *this;
  }
  void construct(pointer p, const T& val) {
    new ((T*)p) T(val);
  }
  void destroy(pointer p) {
    p->~T();
  }

  size_type max_size() const {
    return size_t(-1);
  }

  template <class U>
  struct rebind {
    typedef my_allocator<U> other;
  };

  template <class U>
  my_allocator(const my_allocator<U>&) {
  }

  template <class U>
  my_allocator& operator=(const my_allocator<U>&) {
    return *this;
  }
};
}  // namespace allocator_namespace

void foo() {
  using namespace allocator_namespace;
  std::vector<int, my_allocator<int>> vec_int(11, 0, my_allocator<int>());
}