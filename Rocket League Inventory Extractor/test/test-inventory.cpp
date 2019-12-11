#include <vector>
#include <string>
#include <iostream>

#include "../catch.hpp"
#include "../src/Inventory.h"
#include "../src/InventoryItem.h"

InventoryItem i11("Test Item 1");
InventoryItem i21("Test Item 2", "Black");
InventoryItem i31("Test Item 3", "Acrobat", "Black");
InventoryItem i41("Test Item 4", "Acrobat", "Black", true, 2);
InventoryItem i51("Test Item 5", "Acrobat", "Black", "Rare", true, "Decal", 2, "0.5-10");

std::vector<InventoryItem> items = {i11,i31,i41,i51};

TEST_CASE("GetItems returns all items") {
  Inventory inv(items);
  std::vector<InventoryItem> items_ = inv.GetItems();
  REQUIRE(items.size() == inv.GetItems().size());
  REQUIRE(items[0].GetName() == items_[0].GetName());
}

TEST_CASE("AddItem adds a new item if it doesn't exist") {
  Inventory inv(items);
  int prevSize = inv.GetItems().size();
  inv.AddItem(i21);
  REQUIRE(inv.GetItems().size() == prevSize + 1);
}

TEST_CASE("AddItem updates quantity if item exists") {
    Inventory inv(items);
    std::vector<InventoryItem> items_ = inv.GetItems();
    int prevSize = items_.size();
    int prevQt = items_[3].GetQuantity();
    inv.AddItem(i51);
    items_ = inv.GetItems();
    REQUIRE(items_.size() == prevSize);
    REQUIRE(items_[3].GetQuantity() == prevQt + 1);
}

TEST_CASE("RemoveItem does nothing if item doesn't exist") {
    Inventory inv(items);
    int prevSize = inv.GetItems().size();
    inv.RemoveItem(i21);
    REQUIRE(inv.GetItems().size() == prevSize);
}

TEST_CASE("RemoveItem removes the item if quantity is one") {
    Inventory inv(items);
    int prevSize = inv.GetItems().size();
    inv.RemoveItem(i31);
    REQUIRE(inv.GetItems().size() == prevSize - 1);
}

TEST_CASE("RemoveItem decreases quantity of item") {
    Inventory inv(items);
    inv.AddItem(i31);
    std::vector<InventoryItem> items_ = inv.GetItems();
    int prevSize = items_.size();
    int prevQt = items_[1].GetQuantity();
    inv.RemoveItem(i31);
    items_ = inv.GetItems();
    REQUIRE(items_.size() == prevSize);
    REQUIRE(items_[1].GetQuantity() == prevQt - 1);
}

TEST_CASE("UpdateItemPrice works") {
    Inventory inv(items);
    inv.UpdateItemPrice(i11,"15-32");
    std::vector<InventoryItem> items_ = inv.GetItems();
    REQUIRE(items_[0].GetPriceRange() == "15-32");
}