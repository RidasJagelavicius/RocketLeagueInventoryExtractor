#include "catch.hpp"
#include "InventoryItem.h"

InventoryItem i1("Test Item 1");
InventoryItem i2("Test Item 2", "Black");
InventoryItem i3("Test Item 3", "Acrobat", "Black");
InventoryItem i4("Test Item 4", "Acrobat", "Black", true, 2);
InventoryItem i5("Test Item 5", "Acrobat", "Black", "Rare", true, "Decal", 2,
                 "0.5-10");

TEST_CASE("GetName returns the name of the item") {
    REQUIRE(i1.GetName() == "Test Item 1");
}

TEST_CASE("IsCertified is false for uncertified items") {
    REQUIRE(!i1.IsCertified());
}

TEST_CASE("IsCertified is true for certified items") {
    REQUIRE(i3.IsCertified());
}

TEST_CASE("GetCertification returns "" for uncertified items") {
    REQUIRE(i1.GetCertification() == "");
}

TEST_CASE("GetCertification returns certification of certified items") {
    REQUIRE(i3.GetCertification() == "Acrobat");
}

TEST_CASE("GetColor returns the color of colored items") {
    REQUIRE(i2.GetColor() == "Black");
}

TEST_CASE("GetPriceLowerBound returns the lower bound of the price") {
    double price = i5.GetPriceLowerBound();
    REQUIRE(price < 0.6 && price > 0.4);
}

TEST_CASE("GetPriceUpperBound returns the upper bound of the price") {
    double price = i5.GetPriceUpperBound();
    REQUIRE(price < 11 && price > 9);
}

TEST_CASE("Negative quantities return -1") { 
	i1.UpdateQuantity(-500);
    REQUIRE(i1.GetQuantity() == -1);
}