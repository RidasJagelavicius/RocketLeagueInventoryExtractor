#include "catch.hpp"
#include "ItemDatabase.h"

ItemDatabase test = ItemDatabase("Prices.json");
ItemDatabase invalid = ItemDatabase("not an actual file");

TEST_CASE("A valid link creates a valid ItemDatabase") {
    REQUIRE(test.IsValidDatabase());
}

TEST_CASE("An invalid link creates an invalid ItemDatabase") {
    REQUIRE(!invalid.IsValidDatabase());
}

TEST_CASE(
	"GetPriceOf(1 arg) successfully returns the price of a default variant") {
    REQUIRE(test.GetPriceOf("maverickgxt") == "0.5-1");
}

TEST_CASE(
    "GetPriceOf(1 arg) returns -2 if default color variant does not exist") {
    REQUIRE(test.GetPriceOf("ninjastar") == "-2");
}

TEST_CASE(
    "GetPriceOf successfully sanitizes user input") {
    REQUIRE(test.GetPriceOf("MAVERICK      GXT") == "0.5-1");
}

TEST_CASE(
    "GetPriceOf returns -1 if the item does not exist in the database") {
    REQUIRE(test.GetPriceOf("fakeitem") == "-1");
}

TEST_CASE(
    "GetPriceOf(2 arg) successfully returns the price of a color variant") {
    REQUIRE(test.GetPriceOf("maverickgxt", "white") == "2-3");
}

TEST_CASE(
    "GetPriceOf(2 arg) successfully sanitizes user input") {
    REQUIRE(test.GetPriceOf("MAVERICK      GXT", "Sky Blue") == "0.5-1");
}

TEST_CASE("GetPriceOf(2 arg) returns -2 if color variant does not exist") {
    REQUIRE(test.GetPriceOf("maverickg1", "Sky Blue") == "-2");
    REQUIRE(test.GetPriceOf("ballistic", "black") == "-2");
}

TEST_CASE("GetRarityOf successfully returns the rarity of an item") {
    REQUIRE(test.GetRarityOf("Trigon") == "Blackmarket");
}

TEST_CASE("GetTypeOf successfully returns the type of an item") {
    REQUIRE(test.GetTypeOf("Trigon") == "Decal");
}

TEST_CASE("GetFullNameOf successfully returns the full name of an item") {
    REQUIRE(test.GetFullNameOf("neyoyo") == "NeYoYo");
}