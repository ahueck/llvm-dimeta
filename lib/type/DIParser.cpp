//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DIParser.h"

#include "DIVisitor.h"
#include "DimetaData.h"
#include "support/Logger.h"

#include <cstdlib>
#include <llvm/BinaryFormat/Dwarf.h>

namespace dimeta::diparser {

class DIEventVisitor : public visitor::DINodeVisitor<DIEventVisitor> {
  // TODO visitVariable
 private:
  state::MetaData current_;
  state::MetaStack stack_;
  DIParseEvents& events_;
  struct EnumMeta {
    llvm::DIType* enum_base{nullptr};
    bool is_enum{false};
  };
  EnumMeta enum_data_{};

 public:
  explicit DIEventVisitor(DIParseEvents& events);
  ~DIEventVisitor();

  bool visitBasicType(const llvm::DIBasicType*);

  bool visitDerivedType(const llvm::DIDerivedType*);

  bool visitCompositeType(const llvm::DICompositeType*);

  bool visitNode(const llvm::DINode* node);

  bool visitRecurringCompositeType(const llvm::DICompositeType*);

  void leaveBasicType(const llvm::DIBasicType*);

  void leaveCompositeType(const llvm::DICompositeType*);

  void leaveRecurringCompositeType(const llvm::DICompositeType*);
};

DIEventVisitor::DIEventVisitor(DIParseEvents& events) : events_{events} {
}

DIEventVisitor::~DIEventVisitor() = default;

bool DIEventVisitor::visitBasicType(const llvm::DIBasicType* basic_type) {
  //  state::Entity fundamental_e{state::Entity::freestanding};
  current_.type = const_cast<llvm::DIBasicType*>(basic_type);

  if (current_.is_member) {
    assert(!stack_.empty() && "Free standing fundamental requires empty stack");
    //    fundamental_e = state::Entity::member;
  }
  // create (free-standing) fundamental (finished recursion)
  //  current_.state = fundamental_e;

  if (enum_data_.is_enum) {
    // pattern matches enum types
    //    events.make_enum_member()
    enum_data_.enum_base = current_.type;
    return true;
  }

  events_.make_fundamental(current_);

  return true;
}

bool DIEventVisitor::visitDerivedType(const llvm::DIDerivedType* derived_type) {
  using namespace llvm::dwarf;

  const auto make_member = [&](const auto* derived) {
    current_.member_name   = derived_type->getName();
    current_.is_member     = true;
    current_.member_offset = derived_type->getOffsetInBits() / 8;
    current_.member_size   = derived_type->getSizeInBits() / 8;
    current_.is_member_static = derived->isStaticMember();
  };

  const auto tag = derived_type->getTag();
  switch (tag) {
    case DW_TAG_member: {
      make_member(derived_type);
      break;
    }
    case DW_TAG_variable: {
      // see test ir/02_mpicxx.ll: Datatype has a static member,
      // which is encoded as a variable, and not as a member (variable)
      if (!derived_type->isStaticMember()) {
        LOG_WARNING("Variable is not a static member. " << *derived_type)
      } else {
        make_member(derived_type);
      }
      break;
    }
    case DW_TAG_typedef:
      current_.typedef_names.emplace_back(derived_type->getName());
      break;
    case DW_TAG_inheritance:
      current_.is_base_class = true;
      current_.member_offset = derived_type->getOffsetInBits() / 8;
      break;
    case DW_TAG_pointer_type:
      current_.dwarf_tags.emplace_back(tag);
      // array of pointers:
      if (!current_.arrays.empty()) {
        current_.arrays.back().array_of_pointer = derived_type->getSizeInBits();
      }
      break;
    default:
      current_.dwarf_tags.emplace_back(tag);
  }

  if (tag == DW_TAG_pointer_type && derived_type->getBaseType() == nullptr) {
    // void* pointer has no basetype
    current_.type        = const_cast<llvm::DIDerivedType*>(derived_type);
    current_.is_void_ptr = true;
    events_.make_void_ptr(current_);
    current_ = state::MetaData{};
    return true;
  }

  // FIXME: hacky (current_.clear()) so vtable pointer -> pointer is not applied to vtable type.
  if (derived_type->getName() == "__vtbl_ptr_type") {
    assert(!stack_.empty() && "Vtable requires composite on stack");
    // Create vtbl_ptr_type member to containing compound (first on stack)
    current_.type = const_cast<llvm::DIDerivedType*>(derived_type);
    //    current_.state = state::Entity::member;
    events_.make_vtable(current_);
    current_ = state::MetaData{};
  }

  return true;
}

bool DIEventVisitor::visitNode(const llvm::DINode* node) {
  if (const auto* enumerator = llvm::dyn_cast<llvm::DIEnumerator>(node)) {
    assert(enum_data_.enum_base != nullptr && "Enumerator needs a base type.");
    current_.member_name   = enumerator->getName();
    current_.member_offset = 0;
    current_.member_size   = enum_data_.enum_base->getSizeInBits() / 8;
    current_.type          = enum_data_.enum_base;
    current_.is_member     = true;
    events_.make_enum_member(current_);
  } else if (const auto* sub_range = llvm::dyn_cast<llvm::DISubrange>(node)) {
    assert(!current_.arrays.empty() && "Subrange requires array composite on stack");
    if (sub_range->getCount().is<llvm::ConstantInt*>()) {
      const auto* count = sub_range->getCount().get<llvm::ConstantInt*>();
      auto range_count  = count->getValue().getLimitedValue();
      auto& array       = current_.arrays.back();
      array.subranges.push_back(range_count);
      // LOG_FATAL(range_count);
    }
  }
  return true;
}

bool DIEventVisitor::visitCompositeType(const llvm::DICompositeType* composite_type) {
  // See, e.g., pass/c/stack_struct_array.c:
  if (composite_type->getTag() == llvm::dwarf::DW_TAG_array_type) {
    current_.is_vector = composite_type->isVector();
    current_.dwarf_tags.emplace_back(current_.is_vector ? static_cast<unsigned>(state::CustomDwarfTag::kVector)
                                                        : static_cast<unsigned>(llvm::dwarf::DW_TAG_array_type));
    current_.arrays.emplace_back(
        state::MetaData::ArrayData{composite_type->getSizeInBits(), Extent{0}, {}, composite_type->isVector()});
    // current_.array_size_bits =composite_type->getSizeInBits();
    return true;
  }

  current_.type            = const_cast<llvm::DICompositeType*>(composite_type);
  current_.has_vtable      = composite_type->getVTableHolder() == composite_type;
  current_.is_forward_decl = composite_type->isForwardDecl();

  enum_data_.is_enum = composite_type->getTag() == llvm::dwarf::DW_TAG_enumeration_type;
  //  current_.state      = state::Entity::Undef;  // determined in "leave" function

  events_.make_composite(current_);

  // "open" a composite type parsing (everything in the follow-up depth search gets added to that
  // current composite, such as members).
  // The so-far collected current_ metadata is used when leaving the opened composite:
  // Clear current_, as that meta info applied to the "composite_type"
  stack_.emplace_back(current_);
  current_ = state::MetaData{};

  return true;
}

void DIEventVisitor::leaveCompositeType(const llvm::DICompositeType* composite_type) {
  // See, e.g., pass/c/stack_struct_array.c:
  if (composite_type->getTag() == llvm::dwarf::DW_TAG_array_type) {
    return;
  }

  assert(!stack_.empty() && "Required metadata is missing!");

  //  state::Entity composite_e{state::Entity::freestanding};
  auto meta_for_composite = stack_.pop_back_val();

  if (meta_for_composite.is_member) {
    // add as member with current_meta data, and add it to current back_val() composite type.
    assert(!stack_.empty() && "Member composite requires composite on stack");
    //    composite_e = state::Entity::member;
  }

  if (meta_for_composite.is_base_class) {
    // add as base class to back_val() composite type
    //    assert(composite_e != state::Entity::member);
    assert(!stack_.empty() && "Base composite requires composite on stack");
    //    composite_e = state::Entity::base;
  }

  if (!(meta_for_composite.is_member || meta_for_composite.is_base_class)) {
    // create (free-standing) compound (finished recursion)
    assert(stack_.empty() && "Free standing composite requires empty stack");
    //    assert(composite_e == state::Entity::freestanding);
  }

  //  meta_for_composite.state = composite_e;
  events_.finalize_composite(meta_for_composite);

  enum_data_ = EnumMeta{};
}

void DIEventVisitor::leaveBasicType(const llvm::DIBasicType*) {
  current_ = state::MetaData{};
}

bool DIEventVisitor::visitRecurringCompositeType(const llvm::DICompositeType* recurring_composite) {
  current_.is_recurring = true;
  return this->visitCompositeType(recurring_composite);
}

void DIEventVisitor::leaveRecurringCompositeType(const llvm::DICompositeType* recurring_composite) {
  this->leaveCompositeType(recurring_composite);
}

void visit_node(const llvm::DINode* node, DIParseEvents& event_handler) {
  DIEventVisitor event_visitor{event_handler};
  const bool result = event_visitor.traverseNode(node);
  if (!result) {
    LOG_DEBUG("Did not parse properly: " << log::ditype_str(node));
  }
}

}  // namespace dimeta::diparser
