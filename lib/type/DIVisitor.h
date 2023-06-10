//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIVISITOR_H
#define DIMETA_DIVISITOR_H

#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/Debug.h"

#include <functional>

namespace dimeta {

namespace visitor {

namespace detail {

template <typename Fn>
class ScopeExit {
 private:
  Fn exit_fn_;

 public:
  explicit ScopeExit(Fn&& exit_fn) : exit_fn_(std::forward<Fn>(exit_fn)) {
  }

  ScopeExit(const ScopeExit&)            = delete;
  ScopeExit& operator=(const ScopeExit&) = delete;

  ~ScopeExit() {
    std::invoke(exit_fn_);
  }
};

template <typename Fn>
ScopeExit<Fn> create_scope_exit(Fn&& exit_fn) {
  return ScopeExit<Fn>(std::forward<Fn>(exit_fn));
}

}  // namespace detail

template <typename SubClass>
class DINodeVisitor {
 private:
  template <typename TySink, typename TySource, typename VisitFn>
  bool invoke_if(VisitFn&& visitor, TySource&& type) {
    if (auto* base_t = llvm::dyn_cast<TySink>(type)) {
      if constexpr (std::is_same_v<std::invoke_result_t<VisitFn, decltype(this), decltype(base_t)>, bool>) {
        return std::invoke(std::forward<VisitFn>(visitor), *this, base_t);
      } else {
        std::invoke(std::forward<VisitFn>(visitor), *this, base_t);
        return true;
      }
    }
    return false;
  }

  template <typename T>
  bool invoke_if_any(T&& type) {
    using namespace llvm;
    return invoke_if<DIBasicType>(&DINodeVisitor::traverseBasicType, std::forward<T>(type)) ||
           invoke_if<DIDerivedType>(&DINodeVisitor::traverseDerivedType, std::forward<T>(type)) ||
           invoke_if<DICompositeType>(&DINodeVisitor::traverseCompositeType, std::forward<T>(type)) ||
           invoke_if<DILocalVariable>(&DINodeVisitor::traverseLocalVariable, std::forward<T>(type));
  }

 protected:
  unsigned depth_composite_{0};
  unsigned depth_derived_{0};
  unsigned depth_var_{0};

  [[nodiscard]] inline unsigned depth() const {
    return depth_composite_ + depth_derived_ + depth_var_;
  }

  inline bool followPointer() {
    return false;
  }

 public:
  [[nodiscard]] SubClass& get() {
    return static_cast<SubClass&>(*this);
  }

  [[nodiscard]] const SubClass& get() const {
    return static_cast<const SubClass&>(*this);
  }

  bool traverseLocalVariable(const llvm::DIVariable* var) {
    const auto exit = detail::create_scope_exit([&]() { get().leaveLocalVariable(var); });
    get().enterLocalVariable(var);

    const bool ret = get().visitLocalVariable(var);
    if (!ret) {
      return false;
    }

    const auto* type = var->getType();
    const auto ret_v = get().traverseType(type);
    return ret_v;
  }

  bool visitLocalVariable(const llvm::DIVariable*) {
    return true;
  }

  bool traverseType(const llvm::DIType* type) {
    if (!type) {
      return true;  // FIXME special case, to be observed
    }

    if (!get().visitType(type)) {
      return false;
    }
    return invoke_if_any(type);
  }

  bool visitType(const llvm::DIType*) {
    return true;
  }

  bool traverseBasicType(const llvm::DIBasicType* basic_type) {
    ++depth_var_;
    const auto exit = detail::create_scope_exit([&]() {
      get().leaveBasicType(basic_type);
      assert(depth_var_ > 0);
      --depth_var_;
    });
    get().enterBasicType(basic_type);
    return get().visitBasicType(basic_type);
  }

  bool visitBasicType(const llvm::DIBasicType*) {
    return true;
  }

  bool traverseDerivedType(const llvm::DIDerivedType* derived_type) {
    ++depth_derived_;
    const auto exit = detail::create_scope_exit([&]() {
      get().leaveDerivedType(derived_type);
      assert(depth_derived_ > 0);
      --depth_derived_;
    });
    get().enterDerivedType(derived_type);

    const bool ret = get().visitDerivedType(derived_type);
    if (!ret) {
      return false;
    }

    const bool is_pointer = (derived_type->getTag() == llvm::dwarf::DW_TAG_pointer_type);
    const auto* next_base = derived_type->getBaseType();

    if (!followPointer() && is_pointer && !llvm::isa<llvm::DIDerivedType>(next_base)) {
      // workaround for endless recursion (e.g., pointer points to encapsulating struct)
      // see test pass/cpp/stack_struct_reprod_map_recursion.cpp
      return true;
    }

    const bool ret_v = get().traverseType(next_base);
    return ret_v;
  }

  bool visitDerivedType(const llvm::DIDerivedType*) {
    return true;
  }

  bool traverseCompositeType(const llvm::DICompositeType* composite_type) {
    ++depth_composite_;
    const auto exit = detail::create_scope_exit([&]() {
      get().leaveCompositeType(composite_type);
      assert(depth_composite_ > 0);
      --depth_composite_;
    });
    get().enterCompositeType(composite_type);

    const bool ret = get().visitCompositeType(composite_type);
    if (!ret) {
      return false;
    }

    const bool ret_b = get().traverseType(composite_type->getBaseType());
    if (!ret_b) {
      return false;
    }

    for (auto* eleme : composite_type->getElements()) {
      invoke_if_any(eleme);
    }

    return true;
  }

  bool visitCompositeType(const llvm::DICompositeType*) {
    return true;
  }

  void enterLocalVariable(const llvm::DIVariable*) {
  }
  void leaveLocalVariable(const llvm::DIVariable*) {
  }
  void enterBasicType(const llvm::DIBasicType*) {
  }
  void leaveBasicType(const llvm::DIBasicType*) {
  }
  void enterDerivedType(const llvm::DIDerivedType*) {
  }
  void leaveDerivedType(const llvm::DIDerivedType*) {
  }
  void enterCompositeType(const llvm::DICompositeType*) {
  }
  void leaveCompositeType(const llvm::DICompositeType*) {
  }
};

}  // namespace visitor

namespace util {

inline std::string tag2string(unsigned tag) {
  using namespace llvm::dwarf;
  switch (tag) {
    case DW_TAG_pointer_type:
      return "*";
    case DW_TAG_reference_type:
      return "&";
    case DW_TAG_const_type:
      return "const";
    case DW_TAG_structure_type:
      return "struct";
    case DW_TAG_class_type:
      return "class";
    case DW_TAG_union_type:
      return "union";
    case DW_TAG_array_type:
      return "array";
    case DW_TAG_enumeration_type:
      return "enum";
    default:
      return std::string{TagString(tag)};
  }
}

class DIPrinter : public visitor::DINodeVisitor<DIPrinter> {
 private:
  llvm::raw_ostream& outp_;
  llvm::Optional<const llvm::Module*> module_;

  std::string no_pointer_str(const llvm::Metadata& type) {
    std::string view;
    llvm::raw_string_ostream rso(view);
#if LLVM_VERSION_MAJOR > 13
    type.print(rso, module_.value_or(nullptr));

    if (module_.has_value()) {
      return rso.str();
    }
#else
    type.print(rso, module_.getValueOr(nullptr));

    if (module_.hasValue()) {
      return rso.str();
    }
#endif
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

  bool visitLocalVariable(llvm::DIVariable const* var) {
    outp_ << llvm::left_justify("", width()) << no_pointer_str(*var) << "\n";
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
};

class DISemPrinter : public visitor::DINodeVisitor<DISemPrinter> {
 private:
  llvm::raw_ostream& outp_;
  struct Meta {
    llvm::SmallVector<unsigned, 8> tag_collector;
    bool is_const{false};
    bool const_ptr{false};
    bool const_obj{false};
    unsigned member_offset{0};
    unsigned array_size_bits{0};
    bool is_member{false};
    void clear() {
      tag_collector.clear();
      is_const        = false;
      const_obj       = false;
      const_ptr       = false;
      array_size_bits = 0;
      member_offset   = 0;
      is_member       = false;
    }
  } meta_{};

  [[nodiscard]] unsigned width() const {
    return depth() == 1 ? 0 : depth();
  }

 public:
  DISemPrinter(llvm::raw_ostream& outp) : outp_(outp) {
  }

  bool visitLocalVariable(llvm::DIVariable const* var) {
    outp_ << llvm::left_justify("", width()) << var->getName() << " = \n";
    return true;
  }

  bool visitBasicType(const llvm::DIBasicType* basic_type) {
    if (meta_.is_const && meta_.const_obj) {
      outp_ << "const ";
    }
    outp_ << basic_type->getName();
    for (const auto& tag : meta_.tag_collector) {
      outp_ << tag2string(tag);
    }
    if (meta_.is_const && meta_.const_ptr) {
      outp_ << " const";
    }

    if (meta_.array_size_bits > 0) {
      outp_ << "[" << (meta_.array_size_bits / basic_type->getSizeInBits()) << "]";
    }

    outp_ << ":" << basic_type->getSizeInBits() / 8;
    outp_ << ":" << meta_.member_offset / 8 << "\n";

    return true;
  }

  bool visitDerivedType(const llvm::DIDerivedType* derived_type) {
    using namespace llvm;
    using namespace llvm::dwarf;

    const auto tag = derived_type->getTag();
    if (tag == DW_TAG_inheritance) {
      //      return false;
      outp_ << llvm::left_justify("", width()) << "Inherited::";
    }

    if (tag == DW_TAG_pointer_type || tag == DW_TAG_reference_type) {
      meta_.tag_collector.emplace_back(tag);
      if (meta_.is_const) {
        meta_.const_ptr = true;
      } else {
        meta_.const_obj = true;
      }
    }
    if (tag == DW_TAG_const_type) {
      meta_.is_const = true;
    }

    if (tag == DW_TAG_member) {
      meta_.member_offset = derived_type->getOffsetInBits();
      meta_.is_member     = true;
      if (derived_type->getFlags() & DINode::DIFlags::FlagArtificial) {
        outp_ << llvm::left_justify("", width()) << "Vtable[" << derived_type->getName() << ":"
              << (derived_type->getSizeInBits() / 8) << ":" << (derived_type->getOffsetInBits() / 8) << "]\n";
        return true;
      }
      outp_ << llvm::left_justify("", width()) << derived_type->getName() << " = ";
    }
    return true;
  }

  bool visitCompositeType(const llvm::DICompositeType* composite_type) {
    using namespace llvm;
    using namespace llvm::dwarf;

    const auto tag = composite_type->getTag();

    if (tag == DW_TAG_array_type) {
      meta_.array_size_bits = composite_type->getSizeInBits();
    } else {
      const std::string type = tag2string(tag);

      if (meta_.is_const && meta_.const_obj) {
        outp_ << "const ";
      }

      outp_ << type << " " << composite_type->getName();

      for (const auto& tag : meta_.tag_collector) {
        outp_ << tag2string(tag);
      }
      if (meta_.is_const && meta_.const_ptr) {
        outp_ << " const";
      }

      if (meta_.array_size_bits > 0) {
        outp_ << "[" << (meta_.array_size_bits / composite_type->getSizeInBits()) << "]";
      }

      outp_ << ":" << composite_type->getSizeInBits() / 8 << ":" << composite_type->getElements().size() << " {\n";
      meta_.clear();
    }
    return true;
  }

  void leaveCompositeType(const llvm::DICompositeType* t) {
    using namespace llvm::dwarf;
    if (t->getTag() != DW_TAG_array_type) {
      outp_ << llvm::left_justify("", width()) << "}\n";
      //      llvm::errs() << "v:" << this->depth_var_ << "d:" << this->depth_derived_ << "c:" << this->depth_composite_
      //                   << "w:" << width() << "\n";
    }
  }

  void enterBasicType(const llvm::DIBasicType*) {
    //    outp_ << llvm::left_justify("", width());
  }

  void leaveBasicType(const llvm::DIBasicType*) {
    meta_.clear();
  }
};

}  // namespace util

}  // namespace dimeta

#endif  // DIMETA_DIVISITOR_H
