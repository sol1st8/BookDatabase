#pragma once

#include <algorithm>
#include <flat_map>
#include <functional>
#include <iterator>
#include <random>
#include <string_view>

#include "book_database.hpp"

#include <print>

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::flat_map<std::string_view, size_t, Comparator> hist;
    for (const auto &book : cont) {
        ++hist[std::ref(book.author)];
    }
    return hist;
}

template <BookContainerLike T>
auto calculateGenreRatings(const BookDatabase<T> &cont) {
    struct GenreInfo {
        size_t count = 0;
        double rating = 0.0;
    };

    std::flat_map<Genre, GenreInfo> fmGenreInfo;
    for (auto book : cont) {
        ++fmGenreInfo[book.genre].count;
        fmGenreInfo[book.genre].rating += book.rating;
    }

    std::flat_map<Genre, double> res;
    for (auto it = fmGenreInfo.cbegin(); it != fmGenreInfo.cend(); ++it) {
        res.emplace(it->first, it->second.rating / it->second.count);
    }

    return res;
}

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &cont) {
    return cont.empty() ? 0 : std::accumulate(cont.begin(), cont.end(), 0.0, [](double sum, const Book &book) {
                                  return sum + book.rating;
                              }) / cont.size();
}

template <BookContainerLike T>
auto sampleRandomBooks(const BookDatabase<T> &cont, size_t N) {
    std::vector<std::reference_wrapper<const Book>> res;
    res.reserve(N);
    std::sample(cont.begin(), cont.end(), std::back_inserter(res), N, std::mt19937{std::random_device{}()});
    return res;
}

template <BookContainerLike T, typename Comparator>
auto getTopNBy(BookDatabase<T> &cont, size_t N, Comparator comp) {
    size_t size = std::min(N, cont.size());
    std::nth_element(cont.begin(), cont.begin() + size, cont.end(), comp);
    std::vector<std::reference_wrapper<const Book>> res(cont.begin(), cont.begin() + size);
    return res;
}

}  // namespace bookdb
