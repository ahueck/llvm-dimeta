#ifndef DIMETA_DIPARSER_H
#define DIMETA_DIPARSER_H

#include "llvm/ADT/SmallVector.h"

#include <string>

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

  //  Entity state;

  Extent member_offset{0};
  Extent member_size{0};

  Extent array_size_bits{0};

  Extent vtable_size{0};
  Extent vtable_offset{0};

  std::string member_name;
  std::string typedef_name;

  llvm::SmallVector<unsigned, 8> dwarf_tags;

  bool is_member{false};
  bool is_base_class{false};
  bool has_vtable{false};
  bool is_recurring{false};
  bool is_void_ptr{false};

  void clear() {
    dwarf_tags.clear();
    array_size_bits = 0;
    member_offset   = 0;
    vtable_size     = 0;
    vtable_offset   = 0;
    is_member       = false;
    typedef_name    = "";
    member_name     = "";
    is_base_class   = false;
    has_vtable      = false;
    is_recurring    = false;
    type            = nullptr;
    is_void_ptr     = false;
    //    state           = state::Entity::Undef;
  }
};

using MetaStack = llvm::SmallVector<MetaData, 4>;

}  // namespace state

class DIParseEvents {
 public:
  virtual void make_fundamental(const state::MetaData&)   = 0;
  virtual void make_void_ptr(const state::MetaData&)      = 0;
  virtual void make_vtable(const state::MetaData&)        = 0;
  virtual void make_composite(const state::MetaData&)     = 0;
  virtual void finalize_composite(const state::MetaData&) = 0;
  virtual ~DIParseEvents()                                = default;
};

void visit_node(const llvm::DINode*, DIParseEvents&);

}  // namespace dimeta::diparser

#endif  // DIMETA_DIPARSER_H
