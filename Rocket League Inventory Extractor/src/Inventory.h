#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "InventoryItem.h"

/* Rocket League Inventory API
by Ridas Jagelavicius */

class Inventory{
  public:
    // Default ctor
    Inventory();

    // Custom ctor
    Inventory(std::vector<InventoryItem> items);

    // Returns the total estimated key value (lower bound) of inventory  
    int GetInventoryWorth();

    // Adds an item to the inventory
    // Increases quantity by 1 if item exists
    void AddItem(const InventoryItem & item);

    // Removes an item from the inventory if it exists
    // Decreases quantity by 1 if item exists
    void RemoveItem(const InventoryItem& itemToRemove);

    // Updates the price of an item if it exists
    void UpdateItemPrice(const InventoryItem& itemToUpdate, std::string newPrice);

    // Prints out an easy-to-read list of items
    void PrettyPrint() const;

    // Prints out a list of items with lower bound prices listed
    void PrintBuyingList() const;

    // Prints out a list of items with upper bound prices listed
    void PrintSellingList() const;

    // Return all items
    std::vector<InventoryItem> GetItems();

  private:
    std::vector<InventoryItem> items_; // List of current inventory items
    std::unordered_map<std::string,std::vector<InventoryItem>> typeMap_; // Maps a type (Topper, Antenna) to a vector of that type
    std::vector<InventoryItem>::iterator FindItem(const InventoryItem& item); // Returns an iterator to the passed item or the end if not found
};