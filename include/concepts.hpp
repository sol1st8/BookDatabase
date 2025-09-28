#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = requires(T t) {
    typename T::value_type;
    typename T::size_type;
    typename T::reference;
    typename T::const_reference;
    typename T::iterator;
    typename T::const_iterator;

    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.end() } -> std::same_as<typename T::iterator>;
    { t.cbegin() } -> std::same_as<typename T::const_iterator>;
    { t.cend() } -> std::same_as<typename T::const_iterator>;
    { t.emplace_back(std::declval<typename T::value_type>()) } -> std::same_as<typename T::reference>;
    { t.empty() } -> std::convertible_to<bool>;
    { t.size() } -> std::convertible_to<size_t>;
    t.push_back(std::declval<typename T::value_type>());
};

template <typename T>
concept BookIterator = std::random_access_iterator<T> &&
                       std::same_as<std::iter_value_t<T>, Book> &&
                       std::convertible_to<std::iter_reference_t<T>, const Book&>;

template <typename S, typename I>
concept BookSentinel = std::sentinel_for<S, I>;

template <typename P>
concept BookPredicate = std::predicate<P, Book> || 
                        std::predicate<P, const Book&>;

template <typename C>
concept BookComparator = std::predicate<C, const Book&, const Book&> ||
                         std::predicate<C, Book, Book>;

}  // namespace bookdb