//  llvm-dimeta library
//  Copyright (c) 2022-2024 llvm-dimeta authors
//  Distributed under the BSD 3-Clause license.
//  (See accompanying file LICENSE)
//  SPDX-License-Identifier: BSD-3-Clause
//

#ifndef DIMETA_UTIL_H
#define DIMETA_UTIL_H

#include <functional>
#include <optional>

namespace dimeta::util {

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

namespace detail {
template <typename Container>
struct OptionalBackInsertIterator {
  using iterator_category = std::output_iterator_tag;
  using value_type        = typename Container::value_type;
  using difference_type   = void;
  using pointer           = void;
  using reference         = void;

  explicit OptionalBackInsertIterator(Container& container) : container_(std::addressof(container)) {
  }

  OptionalBackInsertIterator<Container>& operator=(const std::optional<value_type> opt) {
    if (opt) {
      container_->emplace_back(std::move(opt.value()));
    }
    return *this;
  }

  OptionalBackInsertIterator<Container>& operator*() {
    return *this;
  }

  OptionalBackInsertIterator<Container>& operator++() {
    return *this;
  }

  OptionalBackInsertIterator<Container>& operator++(int) {
    return *this;
  }

 protected:
  Container* container_;
};

}  // namespace detail

template <typename Container>
detail::OptionalBackInsertIterator<Container> optional_back_inserter(Container& container) {
  return detail::OptionalBackInsertIterator<Container>(container);
}

}  // namespace dimeta::util

#endif  // DIMETA_UTIL_H
