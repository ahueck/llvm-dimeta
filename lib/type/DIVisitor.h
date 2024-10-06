//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DIVISITOR_H
#define DIMETA_DIVISITOR_H

#include "Util.h"

#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/Debug.h"

#include <functional>
#include <optional>

namespace dimeta::visitor {

template <typename SubClass>
class DINodeVisitor {
 private:
  llvm::SmallPtrSet<const llvm::DINode*, 8> visited_dinodes_;

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
           invoke_if<DILocalVariable>(&DINodeVisitor::traverseVariable, std::forward<T>(type)) ||
           invoke_if<DIGlobalVariable>(&DINodeVisitor::traverseVariable, std::forward<T>(type)) ||
           invoke_if<DIEnumerator>(&DINodeVisitor::traverseNode, std::forward<T>(type)) ||
           invoke_if<DISubrange>(&DINodeVisitor::traverseNode, std::forward<T>(type));
    ;
  }

 protected:
  unsigned depth_composite_{0};
  unsigned depth_derived_{0};
  unsigned depth_var_{0};

  [[nodiscard]] inline unsigned depth() const {
    return depth_composite_ + depth_derived_ + depth_var_;
  }

  inline bool visited_node(const llvm::DINode* node) {
    return visited_dinodes_.find(node) != std::end(visited_dinodes_);
  }

 public:
  [[nodiscard]] SubClass& get() {
    return static_cast<SubClass&>(*this);
  }

  [[nodiscard]] const SubClass& get() const {
    return static_cast<const SubClass&>(*this);
  }

  bool traverseNode(const llvm::DINode* node) {
    //    assert(
    //        (llvm::isa<llvm::DIVariable>(node) || llvm::isa<llvm::DIType>(node) ||
    //        llvm::isa<llvm::DIEnumerator>(node)) && "Can only visit variables or types");
    if (const auto* type = llvm::dyn_cast<llvm::DIType>(node)) {
      return traverseType(type);
    }
    if (const auto* var = llvm::dyn_cast<llvm::DIVariable>(node)) {
      return traverseVariable(var);
    }

    const auto exit = util::create_scope_exit([&]() { get().leaveNode(node); });
    get().enterNode(node);
    const bool ret = get().visitNode(node);
    return ret;
  }

  bool visitNode(const llvm::DINode*) {
    return true;
  }
  void enterNode(const llvm::DINode*) {
  }
  void leaveNode(const llvm::DINode*) {
  }

  bool traverseVariable(const llvm::DIVariable* var) {
    const auto exit = util::create_scope_exit([&]() { get().leaveVariable(var); });
    get().enterVariable(var);

    const bool ret = get().visitVariable(var);
    if (!ret) {
      return false;
    }

    const auto* type = var->getType();
    const auto ret_v = get().traverseType(type);
    return ret_v;
  }

  bool visitVariable(const llvm::DIVariable*) {
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
    const auto exit = util::create_scope_exit([&]() {
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
    const auto exit = util::create_scope_exit([&]() {
      get().leaveDerivedType(derived_type);
      assert(depth_derived_ > 0);
      --depth_derived_;
    });
    get().enterDerivedType(derived_type);

    const bool ret = get().visitDerivedType(derived_type);
    if (!ret) {
      return false;
    }

    const auto* next_base = derived_type->getBaseType();

    const bool ret_v = get().traverseType(next_base);
    return ret_v;
  }

  bool visitDerivedType(const llvm::DIDerivedType*) {
    return true;
  }

  bool traverseRecurringCompositeType(const llvm::DICompositeType* composite_type) {
    ++depth_composite_;
    const auto exit = util::create_scope_exit([&]() {
      get().leaveRecurringCompositeType(composite_type);
      assert(depth_composite_ > 0);
      --depth_composite_;
    });
    get().enterRecurringCompositeType(composite_type);

    const bool ret = get().visitRecurringCompositeType(composite_type);

    return ret;
  }

  bool visitRecurringCompositeType(const llvm::DICompositeType*) {
    return true;
  }

  bool traverseCompositeType(const llvm::DICompositeType* composite_type) {
    const bool visited_already = visited_node(composite_type);

    if (visited_already) {
      return traverseRecurringCompositeType(composite_type);
    }

    ++depth_composite_;
    const auto exit = util::create_scope_exit([&]() {
      get().leaveCompositeType(composite_type);
      assert(depth_composite_ > 0);
      --depth_composite_;
    });

    // see test "c/stack_struct_member_count.c":
    // this avoid endless recursion of structs, but array-types are special as they can "share" references to, e.g.,
    // basictypes
    const bool is_not_array = composite_type->getTag() != llvm::dwarf::DW_TAG_array_type;
    if (is_not_array) {
      visited_dinodes_.insert(composite_type);
    }
    get().enterCompositeType(composite_type);

    const bool ret = get().visitCompositeType(composite_type);
    if (!ret) {
      return false;
    }

    if (!is_not_array) {
      // Parse subranges of arrays first, before determining type of array
      for (auto* eleme : composite_type->getElements()) {
        invoke_if_any(eleme);
      }
    }

    const bool ret_b = get().traverseType(composite_type->getBaseType());
    if (!ret_b) {
      return false;
    }

    if (is_not_array) {
      // For enum: Pass enum members last, traverseType determines the enum member types first
      for (auto* eleme : composite_type->getElements()) {
        invoke_if_any(eleme);
      }
    }

    return true;
  }

  bool visitCompositeType(const llvm::DICompositeType*) {
    return true;
  }

  void enterVariable(const llvm::DIVariable*) {
  }
  void leaveVariable(const llvm::DIVariable*) {
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
  void enterRecurringCompositeType(const llvm::DICompositeType*) {
  }
  void leaveRecurringCompositeType(const llvm::DICompositeType*) {
  }
};

}  // namespace dimeta::visitor

#endif  // DIMETA_DIVISITOR_H
