#pragma once

#include <print>
#include <span>
#include <string>
#include <unordered_set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    using value_type = typename BookContainer::value_type;
    using iterator = typename BookContainer::iterator;
    using const_iterator = typename BookContainer::const_iterator;
    using size_type = typename BookContainer::size_type;
    using reference = typename BookContainer::reference;
    using const_reference = typename BookContainer::const_reference;

    using AuthorContainer = std::unordered_set<std::string, TransparentStringHash, TransparentStringEqual>;

    BookDatabase() = default;

    constexpr BookDatabase(std::initializer_list<value_type> list) {
        books_.reserve(list.size());
        std::for_each(list.begin(), list.end(), [this](const Book &book) { PushBack(book); });
    }

    iterator begin() noexcept { return books_.begin(); }
    const_iterator begin() const noexcept { return books_.cbegin(); }
    iterator end() noexcept { return books_.end(); }
    const_iterator end() const noexcept { return books_.cend(); }

    size_type size() const noexcept { return books_.size(); }
    bool empty() const noexcept { return books_.empty(); }

    reference operator[](size_type pos) noexcept { return books_[pos]; }
    const reference operator[](size_type pos) const noexcept { return books_[pos]; }
    reference at(size_type pos) { return books_.at(pos); }
    const_reference at(size_type pos) const { return books_.at(pos); }

    constexpr void PushBack(const value_type &elem) {
        books_.push_back(elem);
        AddAuthor(books_.back());
    }
    constexpr void PushBack(value_type &&elem) noexcept {
        books_.push_back(std::move(elem));
        AddAuthor(books_.back());
    }

    const std::span<const Book> GetBooks() const noexcept { return books_; }
    const AuthorContainer &GetAuthors() const noexcept { return authors_; }

    template <typename... Args>
    constexpr reference EmplaceBack(Args &&...args) {
        reference ref = books_.emplace_back(std::forward<Args>(args)...);
        AddAuthor(ref);
        return ref;
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

private:
    constexpr bool AddAuthor(value_type &value) {
        auto [it, inserted] = authors_.emplace(value.author);
        value.author = *it;
        return inserted;
    }

    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }

        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
