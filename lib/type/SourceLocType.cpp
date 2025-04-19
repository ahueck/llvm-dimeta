//  llvm-dimeta library
//  Copyright (c) 2022-2025 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#include "DIUtil.h"
#include "Dimeta.h"
#include "DimetaData.h"
#include "DimetaParse.h"
#include "support/Logger.h"

#include "llvm/IR/DebugInfoMetadata.h"

#include <llvm/ADT/STLExtras.h>
#include <optional>

namespace dimeta {

std::optional<location::SourceLocation> location_for(const DimetaData& data) {
  if (data.di_location) {
    auto loc = data.di_location.value();
    return location::SourceLocation{std::string{loc->getFilename()},          //
                                    std::string{loc->getScope()->getName()},  //
                                    loc->getLine()};
  }
  if (!data.di_variable) {
    return {};
  }

  const auto make_source_loc = [](const auto* variable) {
    const auto file           = std::string{variable->getFilename()};
    const auto function_scope = [](const auto alloc) -> std::string {
      const auto* scope = alloc->getScope();
      if (scope) {
        return std::string{scope->getName()};
      }
      return "";
    }(variable);
    return location::SourceLocation{file,            //
                                    function_scope,  //
                                    variable->getLine()};
  };

  if (const auto gv = std::get_if<llvm::DIGlobalVariable*>(&data.di_variable.value())) {
    const auto* global_var = *gv;
    return make_source_loc(global_var);
  }

  if (const auto alloc_var = std::get_if<llvm::DILocalVariable*>(&data.di_variable.value())) {
    const auto* alloc = *alloc_var;
    return make_source_loc(alloc);
  }

  return {};
}

namespace detail {
template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

template <typename Type>
void reset_pointer_qualifier(Type& type, int ptr_level) {
  // "new" can have type_data.pointer_level != metadata pointer level -> if that is the case, we add that qualifier to
  // begin of list
  const auto add_pointer = [&](auto& f) {
    auto count = llvm::count_if(f.qual, [](auto& qual) { return qual == Qualifier::kPtr; });
    if (count < ptr_level) {
      const auto begin = f.qual.begin();
      f.qual.insert(begin, Qualifier{Qualifier::kPtr});
    }
  };
  std::visit(overload{[&](dimeta::QualifiedFundamental& f) -> void { add_pointer(f); },
                      [&](dimeta::QualifiedCompound& q) -> void { add_pointer(q); }},
             type);
}

}  // namespace detail

std::optional<LocatedType> located_type_for(const DimetaData& type_data) {
  auto loc = location_for(type_data);
  if (!loc) {
    LOG_DEBUG("Could not determine source location.");
    return {};
  }

  if (!type_data.entry_type) {
    LOG_DEBUG("Could not determine type (missing entry type).");
    return {};
  }

  assert(type_data.entry_type.has_value() && "Parsing stack type requires entry type.");

  // If a member is the entry type, we ignore that:
  auto* type = type_data.entry_type.value();
  if (const auto* derived_member_maybe = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
    if (di::util::is_member(*derived_member_maybe)) {
      type = derived_member_maybe->getBaseType();
    }
  }

  auto dimeta_result = parser::make_dimetadata(type);
  if (!dimeta_result) {
    return {};
  }

  detail::reset_pointer_qualifier(dimeta_result->type_, type_data.pointer_level);
  return LocatedType{dimeta_result->type_, loc.value()};
}

template <typename IRNode>
std::optional<LocatedType> get_located_type(const IRNode* node) {
  auto type_data = type_for(node);
  if (!type_data) {
    LOG_DEBUG("Could not determine type.");
    return {};
  }
  return located_type_for(type_data.value());
}

std::optional<LocatedType> located_type_for(const llvm::AllocaInst* ai) {
  return get_located_type(ai);
}

std::optional<LocatedType> located_type_for(const llvm::CallBase* cb) {
  return get_located_type(cb);
}

std::optional<LocatedType> located_type_for(const llvm::GlobalVariable* gv) {
  return get_located_type(gv);
}

}  // namespace dimeta