//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DIVisitorUtil.h"

#include "DIVisitor.h"
#include "support/Logger.h"

#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Metadata.h"

#include <optional>

namespace dimeta::visitor::util {

namespace printer {

class DIPrinter : public visitor::DINodeVisitor<DIPrinter> {
 private:
  llvm::raw_ostream& outp_;
  std::optional<const llvm::Module*> module_;

  std::string no_pointer_str(const llvm::Metadata& type) {
    std::string view;
    llvm::raw_string_ostream rso(view);
    type.print(rso, module_.value_or(nullptr));

    if (module_) {
      return rso.str();
    }
    const llvm::StringRef ref(rso.str());
    const auto a_pos = ref.find("=");
    if (a_pos == llvm::StringRef::npos || (a_pos + 2) > ref.size()) {
      return ref.str();
    }

    return std::string{ref.substr(a_pos + 2)};
  }

  [[nodiscard]] unsigned width() const {
    return depth() == 1 ? 0 : depth();
  }

 public:
  explicit DIPrinter(llvm::raw_ostream& outp, const llvm::Module* mod = nullptr) : outp_(outp), module_(mod) {
  }

  bool visitVariable(const llvm::DIVariable* var) {
    outp_ << llvm::left_justify("", width()) << no_pointer_str(*var) << "\n";
    return true;
  }

  bool visitNode(const llvm::DINode* var) {
    outp_ << llvm::left_justify("", width() + 3) << no_pointer_str(*var) << "\n";
    return true;
  }

  bool visitBasicType(const llvm::DIBasicType* basic_type) {
    outp_ << llvm::left_justify("", width() + 3) << no_pointer_str(*basic_type) << "\n";
    return true;
  }

  bool visitDerivedType(const llvm::DIDerivedType* derived_type) {
    outp_ << llvm::left_justify("", width()) << no_pointer_str(*derived_type) << "\n";
    return true;
  }

  bool visitCompositeType(const llvm::DICompositeType* composite_type) {
    outp_ << llvm::left_justify("", width()) << no_pointer_str(*composite_type) << "\n";
    return true;
  }

  bool visitRecurringCompositeType(const llvm::DICompositeType* composite_type) {
    outp_ << llvm::left_justify("", width()) << "**" << no_pointer_str(*composite_type) << "\n";
    return true;
  }
};

}  // namespace printer

void print_dinode(llvm::DINode* node, llvm::raw_ostream& outs, llvm::Module* module) {
  printer::DIPrinter printer{outs, module};
  assert((llvm::isa<llvm::DIVariable>(node) || llvm::isa<llvm::DIType>(node)) && "Can only print variable or type");
  printer.traverseNode(node);
}

namespace detail {

inline bool is_pointer_like(const llvm::DIType& di_type) {
  if (const auto* type = llvm::dyn_cast<llvm::DIDerivedType>(&di_type)) {
    return type->getTag() == llvm::dwarf::DW_TAG_array_type || type->getTag() == llvm::dwarf::DW_TAG_reference_type ||
           type->getTag() == llvm::dwarf::DW_TAG_pointer_type ||
           type->getTag() == llvm::dwarf::DW_TAG_ptr_to_member_type;
  }
  return false;
}
}  // namespace detail

struct DestructureComposite : visitor::DINodeVisitor<DestructureComposite> {
  explicit DestructureComposite(const size_t index) : byte_index_{index} {
  }

  [[nodiscard]] std::optional<StructMember> result() const {
    return this->outermost_candidate_;
  }

  bool visitCompositeType(const llvm::DICompositeType* composite) const {
    LOG_DEBUG("visitCompositeType: " << log::ditype_str(composite) << ": " << composite->getName()
                                     << " index: " << byte_index_ << " offset base: " << this->offset_base_);
    return true;
  }

  bool visitDerivedType(const llvm::DIDerivedType* derived_ty) {
    if (derived_ty->getTag() != llvm::dwarf::DW_TAG_member) {
      return true;
    }
    // assert(derived_ty->getTag() == llvm::dwarf::DW_TAG_member && "Expected member element in composite ty");
    LOG_DEBUG("looking @ member: " << derived_ty->getName() << " offset: " << derived_ty->getOffsetInBits() / 8
                                   << " size: " << derived_ty->getSizeInBits() / 8);

    const auto deriv_offset = (derived_ty->getOffsetInBits() / 8);
    const auto deriv_size   = (derived_ty->getSizeInBits() / 8);
    const auto offset       = this->offset_base_ + deriv_offset;
    const auto lower_bound  = offset;
    const auto upper_bound  = offset + deriv_size;

    if (byte_index_ >= lower_bound && byte_index_ < upper_bound) {
      auto* const member_base_type = derived_ty->getBaseType();

      LOG_DEBUG("saving candidate member type " << log::ditype_str(member_base_type));

      this->outermost_candidate_.emplace(StructMember{const_cast<llvm::DIDerivedType*>(derived_ty), member_base_type});

      if (detail::is_pointer_like(*member_base_type) || member_base_type->getTag() == llvm::dwarf::DW_TAG_array_type) {
        LOG_DEBUG("Terminating recursion, found pointer-like "
                  << detail::is_pointer_like(*member_base_type) << " or array-like "
                  << (member_base_type->getTag() == llvm::dwarf::DW_TAG_array_type))
        return false;  // if offset matches, and its a pointer-like, we do not need to recurse.
      }

      // We should only ever be able to recurse into one composite type where the offset condition holds, so
      // save the offset base for that member.
      if (llvm::isa<llvm::DICompositeType>(member_base_type)) {
        LOG_DEBUG("setting offset base to: " << offset);
        this->offset_base_ = offset;
      }
    }
    return true;
  }

 private:
  size_t byte_index_;
  size_t offset_base_{};
  std::optional<StructMember> outermost_candidate_{};
};

std::optional<StructMember> resolve_byte_offset_to_member_of(llvm::DICompositeType* composite, unsigned offset) {
  DestructureComposite visitor{offset};
  visitor.traverseCompositeType(composite);
  return visitor.result();
}

}  // namespace dimeta::visitor::util
