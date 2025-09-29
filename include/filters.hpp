#pragma once

#include <algorithm>
#include <functional>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {

inline auto YearBetween(int minYear, int maxYear) {
    return [=](const Book &book) { return minYear <= book.year && book.year <= maxYear; };
}

inline auto RatingAbove(double threashold) {
    return [=](const Book &book) { return book.rating > threashold; };
}

inline auto GenreIs(Genre genre) {
    return [=](const Book &book) { return book.genre == genre; };
}

template <typename... Args>
auto all_of(Args... args) {
    return [=](const Book &book) -> bool { return (args(book) && ...); };
}

template <typename... Args>
auto any_of(Args... args) {
    return [=](const Book &book) -> bool { return (args(book) || ...); };
}

template <BookIterator It, BookPredicate Pred>
auto filterBooks(It begin, It end, Pred pred) {
    std::vector<std::reference_wrapper<const Book>> res;
    std::copy_if(begin, end, std::back_inserter(res), pred);
    return res;
}

}  // namespace bookdb