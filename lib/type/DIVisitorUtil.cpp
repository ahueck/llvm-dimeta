//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DIVisitor.h"

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

}  // namespace dimeta::visitor::util
