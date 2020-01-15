#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "InventoryItem.h"
#include "ItemDatabase.h"

/* Rocket League Inventory API
by Ridas Jagelavicius */

class Inventory{
  public:
    // Default ctor
    Inventory();

    /** Custom ctor
        @param path_to_database - Full file path to JSON used to create a Database object
    */
    Inventory(std::string path_to_database);

    /** Custom ctor
        @param items - Vector of InventoryItem objects to populate an Inventory with
        @param path_to_database - Full file path to JSON used to create a
       Database object
    */
    Inventory(std::vector<InventoryItem> items, std::string path_to_database);

    /** Returns the total estimated key value (lower bound) of inventory  
        @return The lower bound estimate of an inventory's value (ex. 3k)
    */
    int GetInventoryWorth();

    /** Adds an item to the inventory
        Increases quantity by 1 if item exists
        @param item - The InventoryItem to add to the Inventory
    */ 
    void AddItem(const InventoryItem & item);

    /** Removes an item from the inventory if it exists
        Decreases quantity by 1 if item exists
        @param itemToRemove - The InventoryItem to remove from the Inventory
     */
    void RemoveItem(const InventoryItem& itemToRemove);

    /** Updates the price of an item if it exists
        @param itemToUpdate - The InventoryItem which will have its price updated
        @param newPrice - The new price range of the item (ex. 40-55)
    */
    void UpdateItemPrice(const InventoryItem& itemToUpdate, std::string newPrice);

    /** Returns an easy-to-read list of items
       In the following format:
       [Color] [Certification] ItemName (Quantity) LowerPrice-UpperPrice k
       @return A formatted list of items and their traits, with headers
    */
    std::string PrettyPrint() const;

    /** Returns a list of items with lower bound prices
       In the following format:
       H: LowerPrice k
       W: [Color] [Certification] ItemName
       @return A formatted list of items and their traits, with headers
    */
    std::string PrintBuyingList() const;

    /** Returns a list of items with upper bound prices listed
      In the following format:
      H: [Color] [Certification] ItemName
      W: UpperPrice k or offer
      @return A formatted list of items and their traits, with headers
    */
    std::string PrintSellingList() const;

    /** Return all items
        @return A vector of all InventoryItem's in items_
    */
    std::vector<InventoryItem> GetItems();

    /** Creates a "saved" folder and stores an inventory for faster retrieval
        @return whether or not the process was successful
    */
    bool WriteInvToFile();

    /** Reads in and populates an inventory from saved/inventory.txt
        @return whether or not the process could be completed successfully (file exists and is valid)
    */
    bool ReadInvFromFile();

  private:
    ItemDatabase database_;
    std::vector<InventoryItem> items_; // List of current inventory items
    std::unordered_map<std::string,std::vector<InventoryItem>> typeMap_; // Maps a type (Topper, Antenna) to a vector of items that are that type
    std::vector<InventoryItem>::iterator FindItem(const InventoryItem& item); // Returns an iterator to the passed item or the end if not found
};