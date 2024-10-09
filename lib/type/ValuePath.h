//  Dimeta library
//  Copyright (c) 2022-2023 Alexander Hück
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_VALUEPATH_H
#define DIMETA_VALUEPATH_H

#include "Util.h"

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <cstddef>
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

  [[nodiscard]] std::optional<const llvm::Value*> value() const {
    if (path_to_value.empty()) {
      return {};
    }
    return *(path_to_value.end() - 1);
  }

  [[nodiscard]] std::optional<const llvm::Value*> previous_value() const {
    if (path_to_value.size() < 2) {
      return {};
    }
    return {*(path_to_value.end() - 2)};
  }

  [[nodiscard]] std::optional<const llvm::Value*> start_value() const {
    if (path_to_value.empty()) {
      return {};
    }
    return *path_to_value.begin();
  }

  [[nodiscard]] std::optional<const llvm::Value*> at(int index) const {
    if (index < path_to_value.size() && index >= 0) {
      return path_to_value[index];
    }
    return {};
  }

  [[nodiscard]] size_t size() const {
    return path_to_value.size();
  }

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

  ValuePath& operator+=(llvm::Value* value) {
    path_to_value.push_back(value);
    return *this;
  }

  ValuePath& operator+=(const ValuePath& other_p) {
    path_to_value.append(std::begin(other_p.path_to_value), std::end(other_p.path_to_value));
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

}  // namespace dimeta::dataflow

#endif
