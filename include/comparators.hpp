#pragma once

#include "book.hpp"
#include <algorithm>

namespace bookdb::comp {

struct LessByAuthor {
    bool operator()(const Book &lhs, const Book &rhs) const {
        return std::ranges::lexicographical_compare(lhs.author, rhs.author);
    }
};

struct LessByRating {
    bool operator()(const Book &lhs, const Book &rhs) const { return lhs.rating > rhs.rating; }
};

struct LessByPopularity {
    bool operator()(const Book &lhs, const Book &rhs) const { return lhs.read_count > rhs.read_count; }
};

}  // namespace bookdb::comp