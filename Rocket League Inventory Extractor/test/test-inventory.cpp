#include <vector>
#include <string>
#include <fstream>

#include "../catch.hpp"
#include "../src/Inventory.h"
#include "../src/InventoryItem.h"

InventoryItem i11("Test Item 1");
InventoryItem i21("Test Item 2", "Black");
InventoryItem i31("Test Item 3", "Acrobat", "Black");
InventoryItem i41("Test Item 4", "Acrobat", "Black", true, 2);
InventoryItem i51("Test Item 5", "Acrobat", "Black", "Rare", true, "Decal", 2, "0.5-10");

std::vector<InventoryItem> items = {i11,i31,i41,i51};
std::string path_to_db =
    "C:\\Users\\Unknown_User\\Documents\\openFrameworks\\apps\\fantastic-"
    "finale-astudent82828211\\Rocket League Inventory Extractor\\Prices.json";
TEST_CASE("GetItems returns all items") {
  Inventory inv(items, path_to_db);
  std::vector<InventoryItem> items_ = inv.GetItems();
  REQUIRE(items.size() == inv.GetItems().size());
  REQUIRE(items[0].GetName() == items_[0].GetName());
}

TEST_CASE("AddItem adds a new item if it doesn't exist") {
  Inventory inv(items, path_to_db);
  int prevSize = inv.GetItems().size();
  inv.AddItem(i21);
  REQUIRE(inv.GetItems().size() == prevSize + 1);
}

TEST_CASE("AddItem updates quantity if item exists") {
    Inventory inv(items, path_to_db);
    std::vector<InventoryItem> items_ = inv.GetItems();
    int prevSize = items_.size();
    int prevQt = items_[3].GetQuantity();
    inv.AddItem(i51);
    items_ = inv.GetItems();
    REQUIRE(items_.size() == prevSize);
    REQUIRE(items_[3].GetQuantity() == prevQt + 1);
}

TEST_CASE("RemoveItem does nothing if item doesn't exist") {
    Inventory inv(items, path_to_db);
    int prevSize = inv.GetItems().size();
    inv.RemoveItem(i21);
    REQUIRE(inv.GetItems().size() == prevSize);
}

TEST_CASE("RemoveItem removes the item if quantity is one") {
    Inventory inv(items, path_to_db);
    int prevSize = inv.GetItems().size();
    inv.RemoveItem(i31);
    REQUIRE(inv.GetItems().size() == prevSize - 1);
}

TEST_CASE("RemoveItem decreases quantity of item") {
    Inventory inv(items, path_to_db);
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
    Inventory inv(items, path_to_db);
    inv.UpdateItemPrice(i11,"15-32");
    std::vector<InventoryItem> items_ = inv.GetItems();
    REQUIRE(items_[0].GetPriceRange() == "15-32");
}

TEST_CASE("WriteInvToFile returns true with a nonempty inventory") {
    Inventory inv(items, path_to_db);
    REQUIRE(inv.WriteInvToFile());
}

TEST_CASE("WriteInvToFile successfully writes an inventory to a file") {
    Inventory inv(items, path_to_db);
    REQUIRE(inv.WriteInvToFile());
    int lines = 0;
    std::string temp;
    std::ifstream input("..//saved//inventory.txt");

    while (!input.eof()) {
      std::getline(input,temp);
      lines++;
    }

    int num_items = items.size();
    int num_traits = 6;

    // Files often have an extra blank line containing a bit flag, hence lines - 1
    REQUIRE(lines - 1 == num_items * num_traits);
}

TEST_CASE("WriteInvToFile returns true with an empty inventory") {
    items = {};
    Inventory inv(items, path_to_db);
    REQUIRE(inv.WriteInvToFile());
}

TEST_CASE("ReadInvFromFile returns true with an nonempty inventory") {
    items = {i11, i31, i41, i51};
    Inventory inv(items, path_to_db);
    inv.WriteInvToFile();

    Inventory inv2(path_to_db);
    REQUIRE(inv2.ReadInvFromFile());
}

TEST_CASE("ReadInvFromFile returns true with an empty inventory") {
    items = {};
    Inventory inv(items, path_to_db);
    inv.WriteInvToFile();

    Inventory inv2(path_to_db);
    REQUIRE(inv2.ReadInvFromFile());
}

TEST_CASE("ReadInvFromFile properly populates an inventory") {
    Inventory inv(items, path_to_db);
    inv.WriteInvToFile();

    Inventory inv2(path_to_db);
    inv2.ReadInvFromFile();

    REQUIRE(inv.GetInventoryWorth() == inv2.GetInventoryWorth());

    std::vector<InventoryItem> inv1Items = inv.GetItems();
    std::vector<InventoryItem> inv2Items = inv2.GetItems();
    REQUIRE(inv1Items.size() == inv2Items.size());

    for (unsigned i = 0; i < inv1Items.size(); i++) {
      REQUIRE(inv1Items.at(i) == inv2Items.at(i));
    }
}

TEST_CASE("ReadInvFromFile clears an existing inventory") {
    Inventory inv(items, path_to_db);
    inv.WriteInvToFile();

    int invWorth = inv.GetInventoryWorth();
    inv.ReadInvFromFile();

    REQUIRE(inv.GetInventoryWorth() == invWorth);
}