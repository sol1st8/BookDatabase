#include <gtest/gtest.h>

#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statsistics.hpp"

using namespace bookdb;

class BookDatabaseTest : public ::testing::Test {
protected:
    BookDatabase<std::vector<Book>> db;

    void SetUp() override {
        db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.0, 190);
        db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 143);
        db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.5, 120);
    }
};

TEST_F(BookDatabaseTest, PushBackBook) {
    const size_t initialBookCount = db.size();
    const size_t initialAuthorCount = db.GetAuthors().size();

    Book testBook{"Test Title", "Test Author", 2025, Genre::Mystery, 4.8, 150};
    db.PushBack(testBook);

    EXPECT_EQ(db.size(), initialBookCount + 1);
    EXPECT_EQ(db.GetAuthors().size(), initialAuthorCount + 1);
    EXPECT_EQ(db.GetBooks().back().title, "Test Title");
    EXPECT_EQ(db.GetBooks().back().author, "Test Author");
}

TEST_F(BookDatabaseTest, EmplaceBackBook) {
    const size_t initialBookCount = db.size();
    const size_t initialAuthorCount = db.GetAuthors().size();

    auto &newBook = db.EmplaceBack("Emplaced Book", "New Author", 2025, Genre::SciFi, 4.9, 200);

    EXPECT_EQ(db.size(), initialBookCount + 1);
    EXPECT_EQ(db.GetAuthors().size(), initialAuthorCount + 1);
    EXPECT_EQ(newBook.title, "Emplaced Book");
    EXPECT_EQ(db.GetBooks().back().author, "New Author");
}

TEST_F(BookDatabaseTest, AuthorsAreUnique) {
    db.Clear();

    db.EmplaceBack("First Book", "Author One", 2023, Genre::Fiction, 4.0, 100);
    db.EmplaceBack("Second Book", "Author Two", 2024, Genre::SciFi, 4.5, 200);
    db.EmplaceBack("Third Book", "Author One", 2025, Genre::Mystery, 4.2, 150);

    const auto &authors = db.GetAuthors();
    ASSERT_EQ(authors.size(), 2);
    EXPECT_TRUE(authors.contains("Author One"));
    EXPECT_TRUE(authors.contains("Author Two"));
}

TEST(EmptyBookDatabaseTest, HandlesEmptyState) {
    BookDatabase<std::vector<Book>> emptyDb;

    EXPECT_TRUE(emptyDb.empty());
    EXPECT_EQ(emptyDb.size(), 0);
    EXPECT_TRUE(emptyDb.GetAuthors().empty());
    EXPECT_TRUE(emptyDb.GetBooks().empty());
}

TEST_F(BookDatabaseTest, HandlesEdgeCases) {
    db.EmplaceBack("Invalid Genre Book", "Test Author", 2025, static_cast<Genre>(999), 2.0, 5);
    EXPECT_EQ(db.GetBooks().back().genre, static_cast<Genre>(999));

    db.EmplaceBack("Empty Author Book", "", 2024, Genre::Unknown, 3.5, 10);
    EXPECT_EQ(db.GetBooks().back().author, "");

    db.EmplaceBack("Negative Rating Book", "Some Author", 2023, Genre::Mystery, -1.0, 5);
    EXPECT_DOUBLE_EQ(db.GetBooks().back().rating, -1.0);
}

TEST_F(BookDatabaseTest, BuildsAuthorHistogram) {
    auto authorHistogram = buildAuthorHistogramFlat(db);

    ASSERT_EQ(authorHistogram.size(), 2);
    EXPECT_EQ(authorHistogram["George Orwell"], 2);
    EXPECT_EQ(authorHistogram["F. Scott Fitzgerald"], 1);
}

TEST_F(BookDatabaseTest, CalculatesGenreRatings) {
    auto genreRatings = calculateGenreRatings(db);

    EXPECT_DOUBLE_EQ(genreRatings[Genre::SciFi], 4.0);
    EXPECT_DOUBLE_EQ(genreRatings[Genre::Fiction], (4.4 + 4.5) / 2);
    EXPECT_DOUBLE_EQ(genreRatings[Genre::Unknown], 0.0);
}

TEST_F(BookDatabaseTest, CalculatesAverageRating) {
    const double expectedAverage = (4.0 + 4.4 + 4.5) / 3;
    EXPECT_DOUBLE_EQ(calculateAverageRating(db), expectedAverage);
}

TEST_F(BookDatabaseTest, SelectsTopNBooks) {
    auto topRatedBooks = getTopNBy(db, 2, comp::LessByRating{});

    ASSERT_EQ(topRatedBooks.size(), 2);
    EXPECT_EQ(topRatedBooks[0].get().title, "1984");
    EXPECT_EQ(topRatedBooks[1].get().title, "Animal Farm");
}

TEST_F(BookDatabaseTest, SamplesRandomBooks) {
    auto randomSample = sampleRandomBooks(db, 2);
    ASSERT_EQ(randomSample.size(), 2);

    for (const auto &bookRef : randomSample) {
        auto it = std::find_if(db.begin(), db.end(), [&](const Book &b) { return b.title == bookRef.get().title; });
        EXPECT_NE(it, db.end());
    }
}

TEST_F(BookDatabaseTest, FiltersBooksByYear) {
    auto filteredBooks = filterBooks(db.begin(), db.end(), YearBetween(1940, 1950));

    ASSERT_EQ(filteredBooks.size(), 2);
    EXPECT_EQ(filteredBooks[0].get().title, "1984");
    EXPECT_EQ(filteredBooks[1].get().title, "Animal Farm");
}

TEST_F(BookDatabaseTest, AppliesCompositeFilter) {
    auto compositeFilter = all_of(YearBetween(1900, 2000), RatingAbove(4.3), GenreIs(Genre::Fiction));

    auto filteredBooks = filterBooks(db.begin(), db.end(), compositeFilter);

    ASSERT_EQ(filteredBooks.size(), 2);
    EXPECT_EQ(filteredBooks[0].get().title, "Animal Farm");
    EXPECT_EQ(filteredBooks[1].get().title, "The Great Gatsby");
}

TEST_F(BookDatabaseTest, PredicateFactoriesWork) {
    // GenreIs
    auto sciFiBooks = filterBooks(db.begin(), db.end(), GenreIs(Genre::SciFi));
    ASSERT_EQ(sciFiBooks.size(), 1);
    EXPECT_EQ(sciFiBooks[0].get().title, "1984");

    // RatingAbove
    auto highRatedBooks = filterBooks(db.begin(), db.end(), RatingAbove(4.4));
    ASSERT_EQ(highRatedBooks.size(), 1);
    EXPECT_EQ(highRatedBooks[0].get().title, "The Great Gatsby");

    // YearBetween
    auto midCenturyBooks = filterBooks(db.begin(), db.end(), YearBetween(1940, 1950));
    ASSERT_EQ(midCenturyBooks.size(), 2);
    EXPECT_EQ(midCenturyBooks[0].get().year, 1949);
    EXPECT_EQ(midCenturyBooks[1].get().year, 1945);
}