#pragma once

#include "future.h"
#include "future_error.h"
#include "shared_state.h"
#include "utils.h"

namespace concurrency {
namespace detail {

template<typename T>
class promise_base {
public:
  future<T> get_future() {
    if (state_.use_count() != 1)
      throw future_error(future_errc::future_already_retrieved);
    return make_future(decay_copy(state_));
  }

  void set_exception(std::exception_ptr error) {
    if (!state_)
      throw future_error(future_errc::no_state);
    state_->set_exception(error);
  }

protected:
  std::shared_ptr<detail::shared_state<T>> state_ = std::make_shared<detail::shared_state<T>>();
};

} // namespace detail

template<typename T>
class promise: public detail::promise_base<T> {
public:
  promise() = default;
  promise(promise&&) noexcept = default;
  promise(const promise&) = delete;

  promise& operator= (const promise&) = delete;
  promise& operator= (promise&& rhs) noexcept;

  ~promise() = default;

  void set_value(const T& val) {
    if (!this->state_)
      throw future_error(future_errc::no_state);
    this->state_->emplace(val);
  }

  void set_value(T&& val) {
    if (!this->state_)
      throw future_error(future_errc::no_state);
    this->state_->emplace(std::move(val));
  }
};

template<>
class promise<void>: public detail::promise_base<void> {
public:
  promise() = default;
  promise(promise&&) noexcept = default;
  promise(const promise&) = delete;

  promise& operator= (const promise&) = delete;
  promise& operator= (promise&& rhs) noexcept = default;

  ~promise() = default;

  void set_value() {
    if (!this->state_)
      throw future_error(future_errc::no_state);
    this->state_->emplace();
  }
};

} // namespace concurrency
