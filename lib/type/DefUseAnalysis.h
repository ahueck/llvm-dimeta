//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_DEFUSEANALYSIS_H
#define DIMETA_DEFUSEANALYSIS_H

#include "Util.h"

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Instructions.h"

#include <algorithm>
#include <functional>

namespace dimeta::dataflow {

struct ValuePath {
  llvm::SmallVector<const llvm::Value*, 16> path_to_value{};

  ValuePath() = default;

  explicit ValuePath(const llvm::Value* start) : path_to_value(1, start) {
  }

  [[nodiscard]] inline bool empty() const {
    return path_to_value.empty();
  }

  [[nodiscard]] inline bool only_start() const {
    return path_to_value.size() == 1;
  }

  [[nodiscard]] const llvm::Value* value() const {
    if (path_to_value.empty()) {
      return nullptr;
    }
    return *(path_to_value.end() - 1);
  }

  [[nodiscard]] const llvm::Value* previous_value() const {
    if (path_to_value.size() < 2) {
      return nullptr;
    }
    return *(path_to_value.end() - 2);
  }

  [[nodiscard]] const llvm::Value* start_value() const {
    if (path_to_value.empty()) {
      return nullptr;
    }
    return *path_to_value.begin();
  }

  //  const llvm::Value* operator[](unsigned index) const {
  //    assert(index < path_to_value.size() && "Index out of bounds!");
  //    return path_to_value[index];
  //  }
  const llvm::Optional<const llvm::Value*> at(int index) const {
    if (index < path_to_value.size() && index >= 0) {
      return path_to_value[index];
    }
    return llvm::None;
  }

  int size() const {
    return path_to_value.size();
  }

  //  [[nodiscard]] llvm::Value* value() {
  //    if (path_to_value.empty()) {
  //      return nullptr;
  //    }
  //    return *path_to_value.begin();
  //  }

  ValuePath operator+(const llvm::Value* value) const {
    ValuePath new_p;
    new_p.path_to_value = this->path_to_value;
    new_p.path_to_value.push_back(value);
    return new_p;
  }

  ValuePath operator+(const llvm::User* value) const {
    ValuePath new_p;
    new_p.path_to_value = this->path_to_value;
    new_p.path_to_value.push_back(value);
    return new_p;
  }

  //  ValuePath operator+(const llvm::Value* value) const {
  //    ValuePath new_p;
  //    new_p.path_to_value = this->path_to_value;
  //    new_p.path_to_value.push_back(value);
  //    return new_p;
  //  }

  ValuePath& operator+=(llvm::Value* value) {
    path_to_value.push_back(value);
    return *this;
  }

  ValuePath& operator+=(const ValuePath& other_p) {
    path_to_value.append(other_p.path_to_value);
    return *this;
  }
};

inline llvm::raw_ostream& operator<<(llvm::raw_ostream& os, const ValuePath& path) {
  const auto& vec = path.path_to_value;
  if (vec.empty()) {
    os << "[]";
    return os;
  }
  const auto to_string = [](const auto& value) -> std::string {
    std::string str_buffer;
    llvm::raw_string_ostream stream(str_buffer);
    stream << value;
    return llvm::StringRef{stream.str()}.trim().str();
  };

  const auto* begin = std::begin(vec);
  os << "[" << to_string(**begin);
  std::for_each(std::next(begin), std::end(vec), [&](const auto* value) {
    os << " --> ";
    os << to_string(*value);
  });
  os << "]";
  return os;
}

struct DefUseChain {
 public:
  enum MatchResult { kContinue = 0, kCancel, kSkip };

 private:
  llvm::SmallVector<ValuePath, 16> working_set_;
  llvm::SmallPtrSet<const llvm::Value*, 16> seen_set_;

  void addToWorkS(const ValuePath& value) {
    if (!value.empty() && seen_set_.find(value.value()) == seen_set_.end()) {
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
        addToWork(value.getValue(), current_path);
      }
    };

    addToWorkS(ValuePath{start});

    while (!working_set_.empty()) {
      const ValuePath user = peek();
      if (user.empty()) {
        continue;
      }
      seen_set_.insert(user.value());

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
        [](const ValuePath& val) -> llvm::Optional<decltype(val.value()->users())> {
          const auto* value = val.value();
          return value->users();
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
