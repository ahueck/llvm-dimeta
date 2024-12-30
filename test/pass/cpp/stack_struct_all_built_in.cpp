// RUN: %cpp-to-llvm %s | %apply-verifier | %filecheck %s

#include <cstddef>
#include <cstdint>

struct AllBuiltInTypes {
  // Integral types
  bool boolean_var;                           // Boolean type
  char char_var;                              // Character type
  signed char signed_char_var;                // Signed character type
  unsigned char unsigned_char_var;            // Unsigned character type
  wchar_t wchar_t_var;                        // Wide character type
  char16_t char16_t_var;                      // 16-bit character type (C++11)
  char32_t char32_t_var;                      // 32-bit character type (C++11)
  short short_var;                            // Short integer type
  unsigned short unsigned_short_var;          // Unsigned short integer type
  int int_var;                                // Integer type
  unsigned int unsigned_int_var;              // Unsigned integer type
  long long_var;                              // Long integer type
  unsigned long unsigned_long_var;            // Unsigned long integer type
  long long long_long_var;                    // Long long integer type (at least 64 bits)
  unsigned long long unsigned_long_long_var;  // Unsigned long long integer type

  // Fixed-width integer types (from <cstdint>)
  std::int8_t int8_var;      // 8-bit signed integer
  std::uint8_t uint8_var;    // 8-bit unsigned integer
  std::int16_t int16_var;    // 16-bit signed integer
  std::uint16_t uint16_var;  // 16-bit unsigned integer
  std::int32_t int32_var;    // 32-bit signed integer
  std::uint32_t uint32_var;  // 32-bit unsigned integer
  std::int64_t int64_var;    // 64-bit signed integer
  std::uint64_t uint64_var;  // 64-bit unsigned integer

  // Floating-point types
  float float_var;              // Single-precision floating point
  double double_var;            // Double-precision floating point
  long double long_double_var;  // Extended-precision floating point

  // Special types
  std::nullptr_t null_ptr_var;  // Null pointer type (C++11)
  size_t size_var;              // Unsigned type for array indexing/size
  ptrdiff_t ptr_diff_var;       // Signed integer type for pointer differences

  // Type to hold alignment of any object (from <cstddef>)
  std::max_align_t max_align_var;
};

void foo() {
  AllBuiltInTypes types;
}

// clang-format off
// CHECK: Name:            AllBuiltInTypes
// CHECK-NEXT: Identifier:      _ZTS15AllBuiltInTypes
// CHECK-NEXT: Type:            struct
// CHECK-NEXT: Extent:          192
// CHECK-NEXT: Sizes:           [ 1, 1, 1, 1, 4, 2, 4, 2, 2, 4, 4, 8, 8, 8, 8, 1,
// CHECK-NEXT:                    1, 2, 2, 4, 4, 8, 8, 4, 8, 16, 8, 8, 8, 32 ]
// CHECK-NEXT: Offsets:         [ 0, 1, 2, 3, 4, 8, 12, 16, 18, 20, 24, 32, 40, 48,
// CHECK-NEXT:                    56, 64, 65, 66, 68, 72, 76, 80, 88, 96, 104, 112,
// CHECK-NEXT:                    128, 136, 144, 160 ]
// CHECK-NEXT: Members:
// CHECK-NEXT:   - Name:            boolean_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: bool, Extent: 1, Encoding: bool }
// CHECK-NEXT:   - Name:            char_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT:   - Name:            signed_char_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: signed char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT:   - Name:            unsigned_char_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: unsigned char, Extent: 1, Encoding: unsigned_char }
// CHECK-NEXT:   - Name:            wchar_t_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: wchar_t, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   - Name:            char16_t_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: char16_t, Extent: 2, Encoding: utf_char }
// CHECK-NEXT:   - Name:            char32_t_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: char32_t, Extent: 4, Encoding: utf_char }
// CHECK-NEXT:   - Name:            short_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: short, Extent: 2, Encoding: signed_int }
// CHECK-NEXT:   - Name:            unsigned_short_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: unsigned short, Extent: 2, Encoding: unsigned_int }
// CHECK-NEXT:   - Name:            int_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:   - Name:            unsigned_int_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: unsigned int, Extent: 4, Encoding: unsigned_int }
// CHECK-NEXT:   - Name:            long_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:   - Name:            unsigned_long_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: {{(long unsigned int)|(unsigned long)}}, Extent: 8, Encoding: unsigned_int }
// CHECK-NEXT:   - Name:            long_long_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: long long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:   - Name:            unsigned_long_long_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: {{(long long unsigned int)|(unsigned long long)}}, Extent: 8, Encoding: unsigned_int }
// CHECK-NEXT:   - Name:            int8_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: signed char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT:       Typedef:         int8_t
// CHECK-NEXT:   - Name:            uint8_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: unsigned char, Extent: 1, Encoding: unsigned_char }
// CHECK-NEXT:       Typedef:         uint8_t
// CHECK-NEXT:   - Name:            int16_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: short, Extent: 2, Encoding: signed_int }
// CHECK-NEXT:       Typedef:         int16_t
// CHECK-NEXT:   - Name:            uint16_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: unsigned short, Extent: 2, Encoding: unsigned_int }
// CHECK-NEXT:       Typedef:         uint16_t
// CHECK-NEXT:   - Name:            int32_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:       Typedef:         int32_t
// CHECK-NEXT:   - Name:            uint32_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: unsigned int, Extent: 4, Encoding: unsigned_int }
// CHECK-NEXT:       Typedef:         uint32_t
// CHECK-NEXT:   - Name:            int64_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:       Typedef:         int64_t
// CHECK-NEXT:   - Name:            uint64_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: {{(long unsigned int)|(unsigned long)}}, Extent: 8, Encoding: unsigned_int }
// CHECK-NEXT:       Typedef:         uint64_t
// CHECK-NEXT:   - Name:            float_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: float, Extent: 4, Encoding: float }
// CHECK-NEXT:   - Name:            double_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT:   - Name:            long_double_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: long double, Extent: 16, Encoding: float }
// CHECK-NEXT:   - Name:            null_ptr_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: 'decltype(nullptr)', Extent: 8, Encoding: nullptr }
// CHECK-NEXT:       Typedef:         nullptr_t
// CHECK-NEXT:   - Name:            size_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: {{(long unsigned int)|(unsigned long)}}, Extent: 8, Encoding: unsigned_int }
// CHECK-NEXT:       Typedef:         size_t
// CHECK-NEXT:   - Name:            ptr_diff_var
// CHECK-NEXT:     Builtin:         true
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Fundamental:     { Name: long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:       Typedef:         ptrdiff_t
// CHECK-NEXT:   - Name:            max_align_var
// CHECK-NEXT:     Builtin:         false
// CHECK-NEXT:     Type:
// CHECK-NEXT:       Compound:
// CHECK-NEXT:         Name:            ''
// CHECK-NEXT:         Identifier:      _ZTS11max_align_t
// CHECK-NEXT:         Type:            struct
// CHECK-NEXT:         Extent:          32
// CHECK-NEXT:         Sizes:           [ 8, 16 ]
// CHECK-NEXT:         Offsets:         [ 0, 16 ]
// CHECK-NEXT:         Members:
// CHECK-NEXT:           - Name:            __clang_max_align_nonce1
// CHECK-NEXT:             Builtin:         true
// CHECK-NEXT:             Type:
// CHECK-NEXT:               Fundamental:     { Name: long long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:           - Name:            __clang_max_align_nonce2
// CHECK-NEXT:             Builtin:         true
// CHECK-NEXT:             Type:
// CHECK-NEXT:               Fundamental:     { Name: long double, Extent: 16, Encoding: float }
// CHECK-NEXT:       Typedef:         max_align_t
