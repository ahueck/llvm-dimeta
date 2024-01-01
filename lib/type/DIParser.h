#ifndef DIMETA_DIPARSER_H
#define DIMETA_DIPARSER_H

#include "DIVisitor.h"

#include "llvm/ADT/STLExtras.h"

#include <string>

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
    //    state           = state::Entity::Undef;
  }
};

using MetaStack = llvm::SmallVector<MetaData, 4>;

}  // namespace state

class DIParseEvents {
 public:
  virtual void make_fundamental(const state::MetaData&)   = 0;
  virtual void make_vtable(const state::MetaData&)        = 0;
  virtual void make_composite(const state::MetaData&)     = 0;
  virtual void finalize_composite(const state::MetaData&) = 0;
  ~DIParseEvents()                                        = default;
};

class DIEventVisitor : public visitor::DINodeVisitor<DIEventVisitor> {
  // TODO visitVariable
 private:
  state::MetaData current_;
  state::MetaStack stack_;
  DIParseEvents& events;

 public:
  explicit DIEventVisitor(DIParseEvents& events);
  ~DIEventVisitor();

  bool visitBasicType(const llvm::DIBasicType*);

  bool visitDerivedType(const llvm::DIDerivedType*);

  bool visitCompositeType(const llvm::DICompositeType*);

  bool visitRecurringCompositeType(const llvm::DICompositeType*);

  void leaveBasicType(const llvm::DIBasicType*);

  void leaveCompositeType(const llvm::DICompositeType*);

  void leaveRecurringCompositeType(const llvm::DICompositeType*);
};

namespace util {}

}  // namespace dimeta::diparser

#endif  // DIMETA_DIPARSER_H
