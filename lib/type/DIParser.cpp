#include "DIParser.h"

namespace dimeta::diparser {

DIEventVisitor::DIEventVisitor(DIParseEvents& events) : events{events} {
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
  events.make_fundamental(current_);

  return true;
}

bool DIEventVisitor::visitDerivedType(const llvm::DIDerivedType* derived_type) {
  using namespace llvm::dwarf;

  const auto tag = derived_type->getTag();
  switch (tag) {
    case DW_TAG_member: {
      current_.member_name   = derived_type->getName();
      current_.is_member     = true;
      current_.member_offset = derived_type->getOffsetInBits() / 8;
      current_.member_size   = derived_type->getSizeInBits() / 8;
      break;
    }
    case DW_TAG_typedef:
      current_.typedef_name = derived_type->getName();
      break;
    case DW_TAG_inheritance:
      current_.is_base_class = true;
      break;
    default:
      current_.dwarf_tags.emplace_back(tag);
  }

  // FIXME: hacky (current_.clear()) so vtable pointer -> pointer is not applied to vtable type.
  if (derived_type->getName() == "__vtbl_ptr_type") {
    assert(!stack_.empty() && "Vtable requires composite on stack");
    // Create vtbl_ptr_type member to containing compound (first on stack)
    current_.type = const_cast<llvm::DIDerivedType*>(derived_type);
    //    current_.state = state::Entity::member;
    events.make_vtable(current_);
    current_.clear();
  }

  return true;
}

bool DIEventVisitor::visitCompositeType(const llvm::DICompositeType* composite_type) {
  // See, e.g., pass/c/stack_struct_array.c:
  if (composite_type->getTag() == llvm::dwarf::DW_TAG_array_type) {
    current_.array_size_bits = composite_type->getSizeInBits();
    return true;
  }

  current_.type       = const_cast<llvm::DICompositeType*>(composite_type);
  current_.has_vtable = composite_type->getVTableHolder() == composite_type;
  //  current_.state      = state::Entity::Undef;  // determined in "leave" function

  events.make_composite(current_);

  // "open" a composite type parsing (everything in the follow-up depth search gets added to that
  // current composite, such as members).
  // The so-far collected current_ metadata is used when leaving the opened composite:
  // Clear current_, as that meta info applied to the "composite_type"
  stack_.emplace_back(current_);
  current_.clear();

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
  events.finalize_composite(meta_for_composite);
}

void DIEventVisitor::leaveBasicType(const llvm::DIBasicType*) {
  current_.clear();
}

bool DIEventVisitor::visitRecurringCompositeType(const llvm::DICompositeType* recurring_composite) {
  current_.is_recurring = true;
  return this->visitCompositeType(recurring_composite);
}

void DIEventVisitor::leaveRecurringCompositeType(const llvm::DICompositeType* recurring_composite) {
  this->leaveCompositeType(recurring_composite);
}

}  // namespace dimeta::diparser