// RUN: %cpp-to-llvm -std=c++20 %s | %apply-verifier | %filecheck %s

#include <cstddef>
#include <cstdint>

struct AllBuiltInTypes {
  bool boolean_var;  // Boolean type

  char char_var;                    // Character type
  signed char signed_char_var;      // Signed character type
  unsigned char unsigned_char_var;  // Unsigned character type
  wchar_t wchar_t_var;              // Wide character type
  char8_t char8_t_var;              // 8-bit character type (C++11)
  char16_t char16_t_var;            // 16-bit character type (C++11)
  char32_t char32_t_var;            // 32-bit character type (C++11)

  short short_var;  // Short integer type
  short int short_int_var;
  signed short short_signed_var;
  signed short int short_signed_int_var;

  unsigned short unsigned_short_var;  // Unsigned short integer type
  unsigned short int unsigned_short_int_var;

  int int_var;  // Integer type
  signed signed_var;
  signed int signed_int_var;

  unsigned int unsigned_int_var;  // Unsigned integer type
  unsigned unsigned_var;

  long long_var;  // Long integer type
  long int long_int_var;
  signed long signed_long_var;
  signed long int signed_long_int_var;

  unsigned long unsigned_long_var;  // Unsigned long integer type
  unsigned long int unsigned_long_int_var;

  long long long_long_var;  // Long long integer type (at least 64 bits)
  long long int long_long_int_var;
  signed long long signed_long_long_var;
  signed long long int signed_long_long_int_var;

  unsigned long long unsigned_long_long_var;  // Unsigned long long integer type
  unsigned long long int unsigned_long_long_int_var;

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

  // c++23
  // std::float16_t float16_t_var;
  // std::float32_t float32_t_var;
  // std::float64_t float64_t_var;
  // std::float128_t float128_t_var;
  // std::bfloat16_t bfloat16_t_var;

  // Special types
  void* void_ptr_var;
  std::nullptr_t null_ptr_var;  // Null pointer type (C++11)
  size_t size_var;              // Unsigned type for array indexing/size
  ptrdiff_t ptr_diff_var;       // Signed integer type for pointer differences
  std::byte byte_var;

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
// CHECK-NEXT: Extent:          288
// CHECK-NEXT: Sizes:           [ 1, 1, 1, 1, 4, 1, 2, 4, 2, 2, 2, 2, 2, 2, 4, 4, 
// CHECK-NEXT:                    4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 1, 
// CHECK-NEXT:                    1, 2, 2, 4, 4, 8, 8, 4, 8, 16, 8, 8, 8, 8, 1, 32 ]
// CHECK-NEXT: Offsets:         [ 0, 1, 2, 3, 4, 8, 10, 12, 16, 18, 20, 22, 24, 26, 
// CHECK-NEXT:                    28, 32, 36, 40, 44, 48, 56, 64, 72, 80, 88, 96, 
// CHECK-NEXT:                    104, 112, 120, 128, 136, 144, 145, 146, 148, 152, 
// CHECK-NEXT:                    156, 160, 168, 176, 184, 192, 208, 216, 224, 232, 
// CHECK-NEXT:                    240, 256 ]
// CHECK-NEXT: Members:
// CHECK-NEXT: - Name:            boolean_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: bool, Extent: 1, Encoding: bool }
// CHECK-NEXT: - Name:            char_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT: - Name:            signed_char_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: signed char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT: - Name:            unsigned_char_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: unsigned char, Extent: 1, Encoding: unsigned_char }
// CHECK-NEXT: - Name:            wchar_t_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: wchar_t, Extent: 4, Encoding: signed_int }
// CHECK-NEXT: - Name:            char8_t_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: char8_t, Extent: 1, Encoding: utf_char }
// CHECK-NEXT: - Name:            char16_t_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: char16_t, Extent: 2, Encoding: utf_char }
// CHECK-NEXT: - Name:            char32_t_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: char32_t, Extent: 4, Encoding: utf_char }
// CHECK-NEXT: - Name:            short_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: short, Extent: 2, Encoding: signed_int }
// CHECK-NEXT: - Name:            short_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: short, Extent: 2, Encoding: signed_int }
// CHECK-NEXT: - Name:            short_signed_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: short, Extent: 2, Encoding: signed_int }
// CHECK-NEXT: - Name:            short_signed_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: short, Extent: 2, Encoding: signed_int }
// CHECK-NEXT: - Name:            unsigned_short_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: unsigned short, Extent: 2, Encoding: unsigned_int }
// CHECK-NEXT: - Name:            unsigned_short_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: unsigned short, Extent: 2, Encoding: unsigned_int }
// CHECK-NEXT: - Name:            int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT: - Name:            signed_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT: - Name:            signed_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT: - Name:            unsigned_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: unsigned int, Extent: 4, Encoding: unsigned_int }
// CHECK-NEXT: - Name:            unsigned_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: unsigned int, Extent: 4, Encoding: unsigned_int }
// CHECK-NEXT: - Name:            long_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT: - Name:            long_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT: - Name:            signed_long_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT: - Name:            signed_long_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT: - Name:            unsigned_long_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: {{(unsigned long|long unsigned int)}}, Extent: 8, Encoding: unsigned_int }
// CHECK-NEXT: - Name:            unsigned_long_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: {{(unsigned long|long unsigned int)}}, Extent: 8, Encoding: unsigned_int }
// CHECK-NEXT: - Name:            long_long_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT: - Name:            long_long_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT: - Name:            signed_long_long_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT: - Name:            signed_long_long_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT: - Name:            unsigned_long_long_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: {{(unsigned long long|long long unsigned int)}}, Extent: 8, Encoding: unsigned_int }
// CHECK-NEXT: - Name:            unsigned_long_long_int_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: {{(unsigned long long|long long unsigned int)}}, Extent: 8, Encoding: unsigned_int }
// CHECK-NEXT: - Name:            int8_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: signed char, Extent: 1, Encoding: signed_char }
// CHECK-NEXT:     Typedef:         int8_t
// CHECK-NEXT: - Name:            uint8_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: unsigned char, Extent: 1, Encoding: unsigned_char }
// CHECK-NEXT:     Typedef:         uint8_t
// CHECK-NEXT: - Name:            int16_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: short, Extent: 2, Encoding: signed_int }
// CHECK-NEXT:     Typedef:         int16_t
// CHECK-NEXT: - Name:            uint16_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: unsigned short, Extent: 2, Encoding: unsigned_int }
// CHECK-NEXT:     Typedef:         uint16_t
// CHECK-NEXT: - Name:            int32_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
// CHECK-NEXT:     Typedef:         int32_t
// CHECK-NEXT: - Name:            uint32_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: unsigned int, Extent: 4, Encoding: unsigned_int }
// CHECK-NEXT:     Typedef:         uint32_t
// CHECK-NEXT: - Name:            int64_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:     Typedef:         int64_t
// CHECK-NEXT: - Name:            uint64_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: {{(unsigned long|long unsigned int)}}, Extent: 8, Encoding: unsigned_int }
// CHECK-NEXT:     Typedef:         uint64_t
// CHECK-NEXT: - Name:            float_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: float, Extent: 4, Encoding: float }
// CHECK-NEXT: - Name:            double_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: double, Extent: 8, Encoding: float }
// CHECK-NEXT: - Name:            long_double_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long double, Extent: 16, Encoding: float }
// CHECK-NEXT: - Name:            void_ptr_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: void, Extent: 8, Encoding: void_ptr }
// CHECK-NEXT:     Qualifiers:      [ ptr ]
// CHECK-NEXT: - Name:            null_ptr_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: 'decltype(nullptr)', Extent: 8, Encoding: nullptr }
// CHECK-NEXT:     Typedef:         nullptr_t
// CHECK-NEXT: - Name:            size_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: {{(unsigned long|long unsigned int)}}, Extent: 8, Encoding: unsigned_int }
// CHECK-NEXT:     Typedef:         size_t
// CHECK-NEXT: - Name:            ptr_diff_var
// CHECK-NEXT:   Builtin:         true
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Fundamental:     { Name: long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:     Typedef:         ptrdiff_t
// CHECK-NEXT: - Name:            byte_var
// CHECK-NEXT:   Builtin:         false
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Compound:
// CHECK-NEXT:       Name:            byte
// CHECK-NEXT:       Identifier:      _ZTSSt4byte
// CHECK-NEXT:       Type:            enum
// CHECK-NEXT:       Extent:          1
// CHECK-NEXT: - Name:            max_align_var
// CHECK-NEXT:   Builtin:         false
// CHECK-NEXT:   Type:
// CHECK-NEXT:     Compound:
// CHECK-NEXT:       Name:            ''
// CHECK-NEXT:       Identifier:      _ZTS11max_align_t
// CHECK-NEXT:       Type:            struct
// CHECK-NEXT:       Extent:          32
// CHECK-NEXT:       Sizes:           [ 8, 16 ]
// CHECK-NEXT:       Offsets:         [ 0, 16 ]
// CHECK-NEXT:       Members:
// CHECK-NEXT:         - Name:            __clang_max_align_nonce1
// CHECK-NEXT:           Builtin:         true
// CHECK-NEXT:           Type:
// CHECK-NEXT:             Fundamental:     { Name: long long{{( int)?}}, Extent: 8, Encoding: signed_int }
// CHECK-NEXT:         - Name:            __clang_max_align_nonce2
// CHECK-NEXT:           Builtin:         true
// CHECK-NEXT:           Type:
// CHECK-NEXT:             Fundamental:     { Name: long double, Extent: 16, Encoding: float }
// CHECK-NEXT:     Typedef:         max_align_t
