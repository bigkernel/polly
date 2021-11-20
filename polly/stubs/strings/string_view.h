#pragma once

#if defined(POLLY_HAVE_STD_STRING_VIEW)
#include <string_view>

namespace polly {
using std::string_view;
} // namespace polly
#else // POLLY_HAVE_STD_STRING_VIEW
#include <cstring>
#include <string>
#include <iterator>
#include <limits>
#include <algorithm>

#include "stubs/base/attributes.h"
#include "stubs/base/check.h"
#include "stubs/base/exception.h"

namespace polly {
template<typename Char, typename Traits>
class basic_string_view {
public:
  // Member types
  using traits_type = Traits;
  using value_type = Char;
  using pointer = Char*;
  using const_pointer = const Char*;
  using reference = Char&;
  using const_reference = const Char&;
  using const_iterator = const Char*;
  using iterator = const_iterator;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = const_reverse_iterator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  static constexpr size_type npos = static_cast<size_type>(-1);

  // Null `basic_string_view` constructor.
  constexpr basic_string_view() noexcept: ptr_(nullptr), len_(0) {}

  // Implicit constructor of a `basic_string_view' from NUL-terminated `str'.
  constexpr explicit basic_string_view(const_pointer s) noexcept
      : ptr_(s), len_(s ? traits_type::length(s) : 0) {}

  // Implicit constructor of a `basic_string_view' from a `const char*' and `n'.
  constexpr basic_string_view(const Char* s, size_t n) noexcept
      : ptr_(s), len_(n) {}

  // Implicit constructors of a `basic_string_view' from `std::basic_string'.
  template<typename Alloc>
  basic_string_view(const std::basic_string<Char, traits_type, Alloc>& s)
      : basic_string_view(s.data(), s.size()) {}

  // constexpr basic_string_view(const basic_string_view&) noexcept = default;
  // basic_string_view& operator=(const basic_string_view&) noexcept = default;

  // Iterator
  constexpr const_iterator begin() const noexcept { return ptr_; }
  constexpr const_iterator end() const noexcept { return ptr_ + len_; }
  constexpr const_iterator cbegin() const noexcept { return begin(); }
  constexpr const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  // Element access
  constexpr const_reference operator[](size_type i) const {
    return POLLY_CONST_ASSERT(i < size()), ptr_[i];
  }

  constexpr const_reference at(size_type i) const {
    return POLLY_EXPECT_TRUE(i < size())
        ? ptr_[i] : (polly::ThrowStdOutOfRange("polly::string_view::at"), ptr_[i]);
  }
  constexpr const_reference front() const {
    return POLLY_CONST_ASSERT(!empty()), ptr_[0];
  }
  constexpr const_reference back() const {
    return POLLY_CONST_ASSERT(!empty()), ptr_[size() - 1];
  }
  constexpr const_pointer data() const noexcept { return ptr_; }

  // Capacity
  constexpr size_type size() const noexcept { return len_; }
  constexpr size_type length() const noexcept { return size(); }
  constexpr size_type max_size() const noexcept { return kMaxSize; }
  constexpr bool empty() const noexcept { return len_ == 0; }

  // Modifiers
  void remove_prefix(size_type n) {
    POLLY_CONST_ASSERT(n <= len_);
    ptr_ += n;
    len_ -= n;
  }

  void remove_suffix(size_type n) {
    POLLY_CONST_ASSERT(n <= len_);
    len_ -= n;
  }

  void swap(basic_string_view& other) noexcept {
    auto tmp = *this;
    *this = other;
    other = tmp;
  }

  // Operations
  size_type copy(Char* buf, size_type n, size_type pos = 0) const {
    if (POLLY_EXPECT_FALSE(pos > len_))
      polly::ThrowStdOutOfRange("polly::string_view::copy");

    n = std::min(len_ - pos, n);
    if (n > 0)
      traits_type::copy(buf, ptr_ + pos, n);
    return n;
  }

  constexpr basic_string_view substr(size_type pos = 0, size_type n = npos) const {
    return POLLY_EXPECT_FALSE(pos > len_)
        ? (polly::ThrowStdOutOfRange("polly::string_view::substr"), basic_string_view())
        : basic_string_view(ptr_ + pos, std::min(n, len_ - pos));
  }

  constexpr int compare(basic_string_view other) const noexcept {
    return CompareHelper(len_, other.len_,
        std::min(len_, other.len_) == 0 ? 0
          : traits_type::compare(ptr_, other.ptr_, std::min(len_, other.len_)));
  }

  constexpr int compare(size_type pos, size_type n, basic_string_view other) const {
    return substr(pos, n).compare(other);
  }

  constexpr int compare(const Char* s) const { return compare(basic_string_view(s)); }
  constexpr int compare(size_type pos, size_type n, const Char* s) const {
    return substr(pos, n).compare(basic_string_view(s));
  }

  // basic_string_view::find

  // Find the first substring equal to the given character sequence.
  // @v      view to search for
  // @pos    position at which to start the search
  // @count  length substring to search for
  // @s      pointer to a character string to search for
  // @ch     character to searchc for
  // @return Position of the first character of the found substring, or `npos'
  // if no such substring is found.
  size_type find(basic_string_view v, size_type pos = 0) const noexcept;
  size_type find(Char c, size_type pos = 0) const noexcept;
  size_type find(const Char* s, size_type pos, size_type count) const {
    return find(basic_string_view(s, count), pos);
  }
  size_type find(const Char* s, size_type pos = 0) const {
    return find(basic_string_view(s), pos);
  }

  // Find the last substring equal to the given character sequence.
  // @v      view to search for
  // @pos    position at which to start the search
  // @count  length substring to search for
  // @s      pointer to a character string to search for
  // @ch     character to searchc for
  // @return Position of the first character of the found substring, or `npos'
  // if no such substring is found.
  size_type rfind(basic_string_view v, size_type pos = npos) const noexcept;
  size_type rfind(Char c, size_type pos = npos) const noexcept;
  size_type rfind(const Char* s, size_type pos, size_type count) const {
    return rfind(basic_string_view(s, count), pos);
  }
  size_type rfind(const Char* s, size_type pos = npos) const {
    return rfind(basic_string_view(s), pos);
  }

  // Find the first character equal to any of the character in the given
  // character sequence.
  // @v      view to search for
  // @pos    position at which to start the search
  // @count  length substring to search for
  // @s      pointer to a character string to search for
  // @ch     character to searchc for
  // @return Position of the first occurence of any character of the substring,
  // or `npos' is no such character is found.
  size_type find_first_of(basic_string_view s, size_type pos = 0) const noexcept;
  size_type find_first_of(Char c, size_type pos = 0) const noexcept {
    return find(c, pos);
  }
  size_type find_first_of(const Char* s, size_type pos,
                                    size_type count) const {
    return find_first_of(string_view(s, count), pos);
  }
  size_type find_first_of(const Char* s, size_type pos = 0) const {
    return find_first_of(string_view(s), pos);
  }

  // Find the last character equal to any of the character in the given
  // character sequence.
  // @v      view to search for
  // @pos    position at which to start the search
  // @count  length substring to search for
  // @s      pointer to a character string to search for
  // @ch     character to searchc for
  // @return Position of the last occurence of any character of the substring,
  // or `npos' is no such character is found.
  size_type find_last_of(basic_string_view s, size_type pos = npos) const noexcept;
  size_type find_last_of(Char c, size_type pos = npos) const noexcept {
    return rfind(c, pos);
  }
  size_type find_last_of(const Char* s, size_type pos, size_type count) const {
    return find_last_of(string_view(s, count), pos);
  }
  size_type find_last_of(const Char* s, size_type pos = npos) const {
    return find_last_of(string_view(s), pos);
  }

  // Find the first character not equal to any of the character in the given
  // character sequence.
  // @v      view to search for
  // @pos    position at which to start the search
  // @count  length substring to search for
  // @s      pointer to a character string to search for
  // @ch     character to searchc for
  // @return Position of the first character not equal to any character of
  // the substring, or `npos' is no such character is found.
  size_type find_first_not_of(basic_string_view s, size_type pos = 0) const noexcept;
  size_type find_first_not_of(Char c, size_type pos = 0) const noexcept;
  size_type find_first_not_of(const Char* s, size_type pos,
                              size_type count) const {
    return find_first_not_of(basic_string_view(s, count), pos);
  }
  size_type find_first_not_of(const Char* s, size_type pos = 0) const {
    return find_first_not_of(basic_string_view(s), pos);
  }

  // Find the last character not equal to any of the character in the given
  // character sequence.
  // @v      view to search for
  // @pos    position at which to start the search
  // @count  length substring to search for
  // @s      pointer to a character string to search for
  // @ch     character to searchc for
  // @return Position of the last character not equal to any character of
  // the substring, or `npos' is no such character is found.
  size_type find_last_not_of(basic_string_view s,
                             size_type pos = npos) const noexcept;
  size_type find_last_not_of(Char c, size_type pos = npos) const noexcept;
  size_type find_last_not_of(const Char* s, size_type pos,
                             size_type count) const {
    return find_last_not_of(string_view(s, count), pos);
  }
  size_type find_last_not_of(const Char* s, size_type pos = npos) const {
    return find_last_not_of(basic_string_view(s), pos);
  }

  template <typename Alloc>
  explicit operator std::basic_string<Char, traits_type, Alloc>() const {
    if (!data()) return {};
    return std::basic_string<Char, traits_type, Alloc>(data(), size());
  }

private:
  static constexpr size_type kMaxSize = std::numeric_limits<difference_type>::max();

  static constexpr int CompareHelper(size_type a, size_type b, int r) {
    return r == 0 ? static_cast<int>(a > b) - static_cast<int>(a < b) : (r < 0 ? -1 : 1);
  }

private:

  const_pointer ptr_;
  size_type len_;
};

using string_view = basic_string_view<char, std::char_traits<char>>;

constexpr bool operator==(string_view a, string_view b) noexcept {
  return a.compare(b) == 0;
}

constexpr bool operator!=(string_view a, string_view b) noexcept {
  return !(a == b);
}

constexpr bool operator<(string_view a, string_view b) noexcept {
  return a.compare(b) < 0;
}

constexpr bool operator>(string_view a, string_view b) noexcept {
  return b < a;
}

constexpr bool operator<=(string_view a, string_view b) noexcept {
  return !(b < a);
}

constexpr bool operator>=(string_view a, string_view b) noexcept {
  return !(a < b);
}

} // namespace polly
#endif // POLLY_HAVE_STD_STRING_VIEW