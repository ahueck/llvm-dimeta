//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DEFUSEANALYSIS_H
#define DIMETA_DEFUSEANALYSIS_H

#include "Util.h"
#include "ValuePath.h"

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <functional>

namespace dimeta::dataflow {

struct DefUseChain {
 public:
  enum MatchResult { kContinue = 0, kCancel, kSkip };

 private:
  llvm::SmallVector<ValuePath, 16> working_set_;
  llvm::SmallPtrSet<const llvm::Value*, 16> seen_set_;

  void addToWorkS(const ValuePath& value) {
    if (!value.empty() && seen_set_.find(value.value().value_or(nullptr)) == seen_set_.end()) {
      working_set_.push_back(value);
    }
  }

  template <typename Range>
  void addToWork(Range&& vals, const ValuePath& current) {
    for (auto value : vals) {
      auto path = current + value;
      addToWorkS(path);
    }
  }

  auto peek() -> ValuePath {
    if (working_set_.empty()) {
      return ValuePath{};
    }
    auto* user_iter = working_set_.end() - 1;
    working_set_.erase(user_iter);
    return *user_iter;
  }

  template <typename ShouldSearchFn, typename SDirectionFn, typename CallBackFn>
  void do_traverse(ShouldSearchFn&& should_search, SDirectionFn&& search, const llvm::Value* start,
                   CallBackFn&& match) {
    //    const auto should_search = [](auto user) {
    //      return llvm::isa<AllowedTy>(user) && !llvm::isa<llvm::ConstantData>(user);
    //    };
    const auto scope_clean = util::create_scope_exit([&]() {
      working_set_.clear();
      seen_set_.clear();
    });

    const auto apply_search_direction = [&](const ValuePath& current_path) {
      auto value = search(current_path);
      if (value) {
        addToWork(value.value(), current_path);
      }
    };

    addToWorkS(ValuePath{start});

    while (!working_set_.empty()) {
      const ValuePath user = peek();
      if (user.empty()) {
        continue;
      }

      const auto current_node = user.value();
      if (current_node) {
        seen_set_.insert(current_node.value());
      }

      if (MatchResult match_v = match(user)) {
        switch (match_v) {
          case kSkip:
            continue;
          case kCancel:
            return;
          default:
            break;
        }
      }

      if (should_search(user)) {
        apply_search_direction(user);
      }
    }
  }

 public:
  template <typename CallBackFn, typename ShouldSearchFn>
  void traverse(const llvm::Value* start, CallBackFn&& match, ShouldSearchFn&& should_search) {
    do_traverse(
        std::forward<ShouldSearchFn>(should_search),
        [](const ValuePath& val) -> std::optional<decltype(val.value().value()->users())> {
          const auto value = val.value();
          if (!value) {
            return {};
          }
          return value.value()->users();
        },
        start, std::forward<CallBackFn>(match));
  }

  template <typename CallBackFn, typename ShouldSearchFn, typename SearchDirFn>
  void traverse_custom(const llvm::Value* start, CallBackFn&& match, ShouldSearchFn&& should_search,
                       SearchDirFn&& search_dir) {
    do_traverse(std::forward<ShouldSearchFn>(should_search), std::forward<SearchDirFn>(search_dir), start,
                std::forward<CallBackFn>(match));
  }
};

}  // namespace dimeta::dataflow

#endif  // DIMETA_DEFUSEANALYSIS_H
