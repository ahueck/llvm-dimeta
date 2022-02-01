//  Dimeta library
//  Copyright (c) 2022-2022 Alexander Hück
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

  ScopeExit(const ScopeExit&) = delete;
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
  bool invoke_if(VisitFn&& visiter, TySource&& type) {
    if (auto* base_t = llvm::dyn_cast<TySink>(type)) {
      if constexpr (std::is_same_v<std::invoke_result_t<VisitFn, decltype(this), decltype(base_t)>, bool>) {
        return std::invoke(std::forward<VisitFn>(visiter), *this, base_t);
      } else {
        std::invoke(std::forward<VisitFn>(visiter), *this, base_t);
        return true;
      }
    }
    return false;
  }

  template <typename T>
  bool invoke_if_any(T&& type) {
    using namespace llvm;
    return invoke_if<DIBasicType>(&DINodeVisitor::visitBasicType, std::forward<T>(type)) ||
           invoke_if<DIDerivedType>(&DINodeVisitor::visitDerivedType, std::forward<T>(type)) ||
           invoke_if<DICompositeType>(&DINodeVisitor::visitCompositeType, std::forward<T>(type));
  }

 protected:
  unsigned depth_composite_{0};
  unsigned depth_derived_{0};

  [[nodiscard]] inline unsigned depth() const {
    return depth_composite_ + depth_derived_;
  }

 public:
  [[nodiscard]] SubClass& get() {
    return static_cast<SubClass&>(*this);
  }

  [[nodiscard]] const SubClass& get() const {
    return static_cast<const SubClass&>(*this);
  }

  bool visit(const llvm::DIType* type) {
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

  void visitBasicType(const llvm::DIBasicType* basic_type) {
    get().enterBasicType();
    get().visitBasicType(basic_type);
    get().leaveBasicType();
  }

  bool visitDerivedType(const llvm::DIDerivedType* derived_type) {
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

    const bool ret_v = visit(derived_type->getBaseType());
    return ret_v;
  }

  bool visitCompositeType(const llvm::DICompositeType* composite_type) {
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

    const bool ret_b = visit(composite_type->getBaseType());
    if (!ret_b) {
      return false;
    }

    for (auto* eleme : composite_type->getElements()) {
      //       llvm::errs() << "Debug: " << *eleme << "\n";
      invoke_if_any(eleme);
    }

    return true;
  }

  void enterBasicType() {
  }
  void leaveBasicType() {
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
    default:
      return TagString(tag);
  }
}

// struct Data;

struct Data {
  bool is_composite;
  bool is_array;
  unsigned array_size;
  llvm::SmallVector<Data*, 4> member;
  llvm::SmallVector<unsigned, 4> tags;
};

class DIPrinter : public visitor::DINodeVisitor<DIPrinter> {
 private:
  llvm::raw_ostream& outp_;

  static std::string no_pointer_str(const llvm::DIType& type) {
    std::string view;
    llvm::raw_string_ostream rso(view);
    type.print(rso);

    llvm::StringRef ref(rso.str());
    const auto a_pos = ref.find("=");
    if (a_pos == llvm::StringRef::npos || (a_pos + 2) > ref.size()) {
      return ref.str();
    }

    return ref.substr(a_pos + 2);
  }

  [[nodiscard]] unsigned width() const {
    return depth() == 1 ? 0 : depth() + depth_derived_;
  }

 public:
  DIPrinter(llvm::raw_ostream& outp) : outp_(outp) {
  }

  void visitBasicType(const llvm::DIBasicType* basic_type) {
    outp_ << llvm::left_justify("", width() + 3) << no_pointer_str(*basic_type) << "\n";
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
    return depth() == 1 ? 0 : depth() + depth_derived_;
  }

 public:
  DISemPrinter(llvm::raw_ostream& outp) : outp_(outp) {
  }

  void visitBasicType(const llvm::DIBasicType* basic_type) {
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
  }

  bool visitDerivedType(const llvm::DIDerivedType* derived_type) {
    using namespace llvm;
    using namespace llvm::dwarf;

    const auto tag = derived_type->getTag();
    if (tag == DW_TAG_inheritance) {
      return false;
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
      std::string type = tag2string(tag);

      if (meta_.is_const && meta_.const_obj) {
        outp_ << "const ";
      }

      outp_ << llvm::left_justify("", (depth_derived_ > 1 ? 0 : (meta_.is_member ? 0 : width()))) << type << " "
            << composite_type->getName();

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
    }
  }

  void leaveBasicType() {
    meta_.clear();
  }
};

}  // namespace util

}  // namespace dimeta

#endif  // DIMETA_DIVISITOR_H
