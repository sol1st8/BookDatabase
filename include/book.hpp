#pragma once

#include <format>
#include <stdexcept>
#include <string_view>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

constexpr Genre GenreFromString(std::string_view s) {
    constexpr std::array genres = {std::pair{"Fiction", Genre::Fiction}, std::pair{"NonFiction", Genre::NonFiction},
                                   std::pair{"SciFi", Genre::SciFi}, std::pair{"Biography", Genre::Biography},
                                   std::pair{"Mystery", Genre::Mystery}};

    auto it = std::find_if(genres.begin(), genres.end(), [s](const auto &pair) { return pair.first == s; });

    return it != genres.end() ? it->second : Genre::Unknown;
}

struct Book {
    // string_view для экономии памяти, чтобы ссылаться на оригинальную строку, хранящуюся в другом контейнере
    std::string_view author;
    std::string title;

    int year;
    Genre genre;
    double rating;
    int read_count;

    constexpr Book(std::string_view title, std::string_view author, int year, std::string_view genre_str, double rating,
                   int read_count)
        : author(author), title(title), year(year), genre(GenreFromString(genre_str)), rating(rating),
          read_count(read_count) {}

    constexpr Book(std::string_view title, std::string_view author, int year, Genre genre, double rating,
                   int read_count)
        : author(author), title(title), year(year), genre(genre), rating(rating), read_count(read_count) {}

    auto operator<=>(const Book &) const = default;
};
}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        std::string genre_str;

        // clang-format off
        using bookdb::Genre;
        switch (g) {
            case Genre::Fiction:    genre_str = "Fiction"; break;
            case Genre::Mystery:    genre_str = "Mystery"; break;
            case Genre::NonFiction: genre_str = "NonFiction"; break;
            case Genre::SciFi:      genre_str = "SciFi"; break;
            case Genre::Biography:  genre_str = "Biography"; break;
            case Genre::Unknown:    genre_str = "Unknown"; break;
            default:
                throw logic_error{"Unsupported bookdb::Genre"};
            }
        // clang-format on
        return format_to(fc.out(), "{}", genre_str);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <>
struct formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book &book, FormatContext &fc) const {
        constexpr auto fmt("Author : {}, title : {}, year : {}, genre : {}, rating : {}, read_count : {}");

        return format_to(fc.out(), fmt, book.author, book.title, book.year, book.genre, book.rating, book.read_count);
    }

    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
};

}  // namespace std
