// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 -yaml | %filecheck %s

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

// clang-format off
// CHECK: File:{{.*}}stack_std_vector_allocator.cpp
// CHECK-NEXT: Function:        foo
// CHECK-NEXT:   Line:            76
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            'vector<int, allocator_namespace::my_allocator<int> >'
// CHECK-NEXT:     Identifier:      _ZTSSt6vectorIiN19allocator_namespace12my_allocatorIiEEE
// CHECK-NEXT:     Type:            class
// CHECK-NEXT:     Extent:          32
// CHECK-NEXT:     Base:
// CHECK-NEXT:       - BaseClass:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            '_Vector_base<int, allocator_namespace::my_allocator<int> >'
// CHECK-NEXT:             Identifier:      _ZTSSt12_Vector_baseIiN19allocator_namespace12my_allocatorIiEEE
// CHECK-NEXT:             Type:            struct
// CHECK-NEXT:             Extent:          32
// CHECK-NEXT:             Sizes: [ 32 ] 
// CHECK-NEXT:             Offsets: [ 0 ]
// CHECK-NEXT:             Members:
// CHECK-NEXT:               - Name:            _M_impl
// CHECK-NEXT:                 Builtin:         false
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Compound:
// CHECK-NEXT:                     Name:            _Vector_impl
// CHECK-NEXT:                     Identifier:      _ZTSNSt12_Vector_baseIiN19allocator_namespace12my_allocatorIiEEE12_Vector_implE
// CHECK-NEXT:                     Type:            struct
// CHECK-NEXT:                     Extent:          32
// CHECK-NEXT:                     Base:
// CHECK-NEXT:                       - BaseClass:
// CHECK-NEXT:                           Compound:
// CHECK-NEXT:                             Name:            'my_allocator<int>'
// CHECK-NEXT:                             Identifier:      _ZTSN19allocator_namespace12my_allocatorIiEE
// CHECK-NEXT:                             Type:            class
// CHECK-NEXT:                             Extent:          8
// CHECK-NEXT:                             Sizes:           [ 8 ]
// CHECK-NEXT:                             Offsets:         [ 0 ]
// CHECK-NEXT:                             Members:
// CHECK-NEXT:                               - Name:            padding
// CHECK-NEXT:                                 Builtin:         true
// CHECK-NEXT:                                 Type:
// CHECK-NEXT:                                   Fundamental:     { Name: void, Extent: 8, 
// CHECK-NEXT:                                                      Encoding: void_ptr }
// CHECK-NEXT:                                   Qualifiers:      [ ptr ]
// CHECK-NEXT:                           Typedef:         _Tp_alloc_type
// CHECK-NEXT:                       - BaseClass:
// CHECK-NEXT:                           Compound:
// CHECK-NEXT:                             Name:            _Vector_impl_data
// CHECK-NEXT:                             Identifier:      _ZTSNSt12_Vector_baseIiN19allocator_namespace12my_allocatorIiEEE17_Vector_impl_dataE
// CHECK-NEXT:                             Type:            struct
// CHECK-NEXT:                             Extent:          24
// CHECK-NEXT:                             Sizes:           [ 8, 8, 8 ]
// CHECK-NEXT:                             Offsets:         [ 0, 8, 16 ]
// CHECK:                      Offset:          8



// *** Dumping AST Record Layout
//          0 | class allocator_namespace::my_allocator<int>
//          0 |   void * padding
//            | [sizeof=8, dsize=8, align=8,
//            |  nvsize=8, nvalign=8]
// *** Dumping AST Record Layout
//          0 | struct std::_Vector_base<int, class allocator_namespace::my_allocator<int> >::_Vector_impl_data
//          0 |   pointer _M_start
//          8 |   pointer _M_finish
//         16 |   pointer _M_end_of_storage
//            | [sizeof=24, dsize=24, align=8,
//            |  nvsize=24, nvalign=8] 
// *** Dumping AST Record Layout
//          0 | struct std::_Vector_base<int, class allocator_namespace::my_allocator<int> >::_Vector_impl
//          0 |   class allocator_namespace::my_allocator<int> (base)
//          0 |     void * padding
//          8 |   struct std::_Vector_base<int, class allocator_namespace::my_allocator<int> >::_Vector_impl_data (base)
//          8 |     pointer _M_start
//         16 |     pointer _M_finish
//         24 |     pointer _M_end_of_storage
//            | [sizeof=32, dsize=32, align=8,
//            |  nvsize=32, nvalign=8]
// *** Dumping AST Record Layout
//          0 | struct std::_Vector_base<int, class allocator_namespace::my_allocator<int> >
//          0 |   struct std::_Vector_base<int, class allocator_namespace::my_allocator<int> >::_Vector_impl _M_impl
//          0 |     class allocator_namespace::my_allocator<int> (base)
//          0 |       void * padding
//          8 |     struct std::_Vector_base<int, class allocator_namespace::my_allocator<int> >::_Vector_impl_data (base)
//          8 |       pointer _M_start
//         16 |       pointer _M_finish
//         24 |       pointer _M_end_of_storage
//            | [sizeof=32, dsize=32, align=8,
//            |  nvsize=32, nvalign=8]
// *** Dumping AST Record Layout
//          0 | class std::vector<int, class allocator_namespace::my_allocator<int> >
//          0 |   struct std::_Vector_base<int, class allocator_namespace::my_allocator<int> > (base)
//          0 |     struct std::_Vector_base<int, class allocator_namespace::my_allocator<int> >::_Vector_impl _M_impl
//          0 |       class allocator_namespace::my_allocator<int> (base)
//          0 |         void * padding
//          8 |       struct std::_Vector_base<int, class allocator_namespace::my_allocator<int> >::_Vector_impl_data (base)
//          8 |         pointer _M_start
//         16 |         pointer _M_finish
//         24 |         pointer _M_end_of_storage
//            | [sizeof=32, dsize=32, align=8,
//            |  nvsize=32, nvalign=8]
