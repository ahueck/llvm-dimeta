#ifndef DIMETA_DIPARSER_H
#define DIMETA_DIPARSER_H

#include "llvm/ADT/SmallVector.h"

#include <cstdint>
#include <string>
#include <vector>

namespace llvm {
class DINode;
class DIType;
}  // namespace llvm

namespace dimeta::diparser {

using Extent = std::uint64_t;

namespace state {
// enum class Entity : uint8_t { Undef = 0, freestanding = 1, member, base };

struct MetaData {
  llvm::DIType* type;

  Extent member_offset{0};
  Extent member_size{0};

  struct ArrayData {
    Extent array_size_bits{0};
    Extent array_of_pointer{0};
    std::vector<Extent> subranges;
  };

  std::vector<ArrayData> arrays;

  Extent vtable_size{0};
  Extent vtable_offset{0};

  std::string member_name;
  std::vector<std::string> typedef_names;

  llvm::SmallVector<unsigned, 8> dwarf_tags;
  bool is_member{false};
  bool is_base_class{false};
  bool has_vtable{false};
  bool is_recurring{false};
  bool is_void_ptr{false};
  bool is_vector{false};
  bool is_forward_decl{false};
};

using MetaStack = llvm::SmallVector<MetaData, 4>;

}  // namespace state

class DIParseEvents {
 public:
  virtual void make_fundamental(const state::MetaData&)   = 0;
  virtual void make_void_ptr(const state::MetaData&)      = 0;
  virtual void make_vtable(const state::MetaData&)        = 0;
  virtual void make_enum_member(const state::MetaData&)   = 0;
  virtual void make_composite(const state::MetaData&)     = 0;
  virtual void finalize_composite(const state::MetaData&) = 0;
  virtual ~DIParseEvents()                                = default;
};

void visit_node(const llvm::DINode*, DIParseEvents&);

}  // namespace dimeta::diparser

#endif  // DIMETA_DIPARSER_H
