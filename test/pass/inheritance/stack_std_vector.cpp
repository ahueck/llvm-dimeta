// RUN: %cpp-to-llvm %s | %apply-verifier 2>&1 -yaml | %filecheck %s

#include <vector>

void foo() {
  std::vector<double> vec_double;  // = {1., 2.};
  //  return vec_double[0];
}

// clang-format off
// CHECK:   File:{{.*}}stack_std_vector.cpp
// CHECK-NEXT:   Function:        foo
// CHECK-NEXT:   Line:            6
// CHECK-NEXT: Builtin:         false
// CHECK-NEXT: Type:
// CHECK-NEXT:   Compound:
// CHECK-NEXT:     Name:            'vector<double, std::allocator<double> >'
// CHECK-NEXT:     Identifier:      _ZTSSt6vectorIdSaIdEE
// CHECK-NEXT:     Type:            class
// CHECK-NEXT:     Extent:          24
// CHECK-NEXT:     Base:
// CHECK-NEXT:       - BaseClass:
// CHECK-NEXT:           Compound:
// CHECK-NEXT:             Name:            '_Vector_base<double, std::allocator<double> >'
// CHECK-NEXT:             Identifier:      _ZTSSt12_Vector_baseIdSaIdEE
// CHECK-NEXT:             Type:            struct
// CHECK-NEXT:             Extent:          24
// CHECK-NEXT:             Sizes:           [ 24 ]
// CHECK-NEXT:             Offsets:         [ 0 ]
// CHECK-NEXT:             Members:
// CHECK-NEXT:               - Name:            _M_impl
// CHECK-NEXT:                 Builtin:         false
// CHECK-NEXT:                 Type:
// CHECK-NEXT:                   Compound:
// CHECK-NEXT:                     Name:            _Vector_impl
// CHECK-NEXT:                     Identifier:      _ZTSNSt12_Vector_baseIdSaIdEE12_Vector_implE
// CHECK-NEXT:                     Type:            struct
// CHECK-NEXT:                     Extent:          24
// CHECK-NEXT:                     Base:
// CHECK-NEXT:                       - BaseClass:
// CHECK-NEXT:                           Compound:
// CHECK-NEXT:                             Name:            '{{.*}}allocator<double>'
// CHECK-NEXT:                             Identifier:      {{.*}}
// CHECK-NEXT:                             Type:            class
// CHECK-NEXT:                             Extent:          1
// CHECK-NEXT:                             Base:
// CHECK-NEXT:                               - BaseClass:
// CHECK-NEXT:                                   Compound:
// CHECK-NEXT:                                     Name:            '{{.*}}new_allocator<double>'
// CHECK-NEXT:                                     Identifier:      {{.*}}
// CHECK-NEXT:                                     Type:            class
// CHECK-NEXT:                                     Extent:          1
// CHECK-NEXT:                                   Typedef:         '__allocator_base<double>'
// CHECK-NEXT:                             EBO:             true
// CHECK-NEXT:                           Typedef:         _Tp_alloc_type
// CHECK-NEXT:                       EBO:             true
// CHECK-NEXT:                       - BaseClass:
// CHECK-NEXT:                           Compound:
// CHECK-NEXT:                             Name:            _Vector_impl_data
// CHECK-NEXT:                             Identifier:      _ZTSNSt12_Vector_baseIdSaIdEE17_Vector_impl_dataE
// CHECK-NEXT:                             Type:            struct
// CHECK-NEXT:                             Extent:          24
// CHECK-NEXT:                             Sizes:           [ 8, 8, 8 ]
// CHECK-NEXT:                             Offsets:         [ 0, 8, 16 ]
// CHECK-NEXT:                             Members:
// CHECK-NEXT:                               - Name:            _M_start
// CHECK-NEXT:                                 Builtin:         true
// CHECK-NEXT:                                 Type:
// CHECK-NEXT:                                   Fundamental:     { Name: double, Extent: 8, 
// CHECK-NEXT:                                                      Encoding: float }
// CHECK-NEXT:                                   Qualifiers:      [ ptr ]
// CHECK-NEXT:                                   Typedef:         pointer
// CHECK-NEXT:                               - Name:            _M_finish
// CHECK-NEXT:                                 Builtin:         true
// CHECK-NEXT:                                 Type:
// CHECK-NEXT:                                   Fundamental:     { Name: double, Extent: 8, 
// CHECK-NEXT:                                                      Encoding: float }
// CHECK-NEXT:                                   Qualifiers:      [ ptr ]
// CHECK-NEXT:                                   Typedef:         pointer
// CHECK-NEXT:                               - Name:            _M_end_of_storage
// CHECK-NEXT:                                 Builtin:         true
// CHECK-NEXT:                                 Type:
// CHECK-NEXT:                                   Fundamental:     { Name: double, Extent: 8, 
// CHECK-NEXT:                                                      Encoding: float }
// CHECK-NEXT:                                   Qualifiers:      [ ptr ]
// CHECK-NEXT:                                   Typedef:         pointer


// *** Dumping AST Record Layout
//          0 | class std::allocator<double> (empty)
//          0 |   class std::__new_allocator<double> (base) (empty)
//            | [sizeof=1, dsize=0, align=1,
//            |  nvsize=1, nvalign=1]
// *** Dumping AST Record Layout
//          0 | struct std::_Vector_base<double, class std::allocator<double> >::_Vector_impl_data
//          0 |   pointer _M_start
//          8 |   pointer _M_finish
//         16 |   pointer _M_end_of_storage
//            | [sizeof=24, dsize=24, align=8,
//            |  nvsize=24, nvalign=8]
// *** Dumping AST Record Layout
//          0 | struct std::_Vector_base<double, class std::allocator<double> >::_Vector_impl
//          0 |   class std::allocator<double> (base) (empty)
//          0 |     class std::__new_allocator<double> (base) (empty)
//          0 |   struct std::_Vector_base<double, class std::allocator<double> >::_Vector_impl_data (base)
//          0 |     pointer _M_start
//          8 |     pointer _M_finish
//         16 |     pointer _M_end_of_storage
//            | [sizeof=24, dsize=24, align=8,
//            |  nvsize=24, nvalign=8]
// *** Dumping AST Record Layout
//          0 | struct std::_Vector_base<double, class std::allocator<double> >
//          0 |   struct std::_Vector_base<double, class std::allocator<double> >::_Vector_impl _M_impl
//          0 |     class std::allocator<double> (base) (empty)
//          0 |       class std::__new_allocator<double> (base) (empty)
//          0 |     struct std::_Vector_base<double, class std::allocator<double> >::_Vector_impl_data (base)
//          0 |       pointer _M_start
//          8 |       pointer _M_finish
//         16 |       pointer _M_end_of_storage
//            | [sizeof=24, dsize=24, align=8,
//            |  nvsize=24, nvalign=8]
// *** Dumping AST Record Layout
//          0 | class std::vector<double>
//          0 |   struct std::_Vector_base<double, class std::allocator<double> > (base)
//          0 |     struct std::_Vector_base<double, class std::allocator<double> >::_Vector_impl _M_impl
//          0 |       class std::allocator<double> (base) (empty)
//          0 |         class std::__new_allocator<double> (base) (empty)
//          0 |       struct std::_Vector_base<double, class std::allocator<double> >::_Vector_impl_data (base)
//          0 |         pointer _M_start
//          8 |         pointer _M_finish
//         16 |         pointer _M_end_of_storage
//            | [sizeof=24, dsize=24, align=8,
//            |  nvsize=24, nvalign=8]
