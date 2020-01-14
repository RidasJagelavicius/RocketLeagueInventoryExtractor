#include <vector>
#include <string>
#include <cmath>
#include <sstream>
#include <filesystem>
#include <fstream>

#include "Inventory.h"

/* Rocket League Inventory API
by Ridas Jagelavicius */

// Default ctor
Inventory::Inventory() {
/* Nothing */
}

// Custom ctor
Inventory::Inventory(std::string path_to_database) {
    database_ = ItemDatabase(path_to_database);
}

// Custom ctor
Inventory::Inventory(std::vector<InventoryItem> items,
                     std::string path_to_database) {
 for (const InventoryItem & item : items) {
  AddItem(item);
 }
 database_ = ItemDatabase(path_to_database);
}

// Returns the total estimated key value (lower bound) of inventory
int Inventory::GetInventoryWorth() {
  double worth = 0;

  // Loop through each item and append its lower bound price
  for(const InventoryItem & item : items_) {
    worth += item.GetPriceLowerBound();
  }

  return std::round(worth);
}

// Adds an item to the inventory
void Inventory::AddItem(const InventoryItem& item) {
    
    // Find and update quantity if item exists
    std::vector<InventoryItem>::iterator it = FindItem(item);

    // Add to typeMap
    std::string type = database_.GetTypeOf(item.GetName());
    if (type != "-1" && type != "-2") {
      
      std::unordered_map<std::string, std::vector<InventoryItem>>::iterator
      it2 = typeMap_.find(type);

      if (it2 != typeMap_.end()) {
        // If vector exists, add to it
        std::vector<InventoryItem> & itemsOfType = it2->second;

        bool wasFound = false;
        // Update quantity if item already exists in vector
        for (std::vector<InventoryItem>::iterator iter = itemsOfType.begin();
        iter != itemsOfType.end(); ++iter) {
          
            if (*iter == item) {
              int quantity = iter->GetQuantity();
              iter->UpdateQuantity(quantity + 1);
              wasFound = true;
            }
          }

        if (!wasFound)
          itemsOfType.push_back(item);
      } else {
        // If vector does not exist, create it
        std::vector<InventoryItem> itemsOfType;
        itemsOfType.push_back(item);
        typeMap_[type] = itemsOfType;
      }
    }

    // Update item list
    if (it != items_.end()) {
        int quantity = it->GetQuantity();
        it->UpdateQuantity(quantity + 1);
    } else {
        // Otherwise, add the item
        items_.push_back(item);
    }
}

// Removes an item from the inventory if it exists
void Inventory::RemoveItem(const InventoryItem& itemToRemove) {
  
  std::vector<InventoryItem>::iterator it = FindItem(itemToRemove);

  // Remove from typeMap
  std::string type = database_.GetTypeOf(itemToRemove.GetName());
  if (type != "-1" && type != "-2") {
      std::unordered_map<std::string, std::vector<InventoryItem>>::iterator it2 =
          typeMap_.find(type);

      if (it2 != typeMap_.end()) {
          // If vector exists, remove from it
          std::vector<InventoryItem>& itemsOfType = it2->second;

          // Update quantity if item already exists in vector or remove
          for (std::vector<InventoryItem>::iterator iter = itemsOfType.begin();
               iter != itemsOfType.end(); ++iter) {
              if (*iter == itemToRemove) {
                  int quantity = iter->GetQuantity();

                  // Remove item or decrease quantity
                  if (quantity > 1) {
                      iter->UpdateQuantity(quantity - 1);
                  } else {
                      typeMap_.erase(it2);  
                  }
                  break;
              }
          }
      }
  }

  // Find and remove item if it exists or update quantity
  if (it != items_.end()) {
      int quantity = it->GetQuantity();

      // Remove item or decrease quantity
      if (quantity > 1) {
          it->UpdateQuantity(quantity - 1);
      } else {
          items_.erase(it);
      }
  }
}

// Updates the price of an item if it exists
void Inventory::UpdateItemPrice(const InventoryItem& itemToUpdate,
                                std::string newPrice) {
    // Update items_
    std::vector<InventoryItem>::iterator it = FindItem(itemToUpdate);
    if (it != items_.end())
      it->UpdatePrice(newPrice);

    // Update typeMap_
    std::string type = itemToUpdate.GetType();
    std::unordered_map<std::string, std::vector<InventoryItem>>::iterator it2 =
        typeMap_.find(type);

    if (it2 != typeMap_.end()) {
        // If vector exists, remove from it
        std::vector<InventoryItem>& itemsOfType = it2->second;

        // Update quantity if item already exists in vector or remove
        for (std::vector<InventoryItem>::iterator iter = itemsOfType.begin();
             iter != itemsOfType.end(); ++iter) {
            if (*iter == itemToUpdate) {
                iter->UpdatePrice(newPrice);
                break;
            }
        }
    }
}

// Returns an easy-to-read list of items
std::string Inventory::PrettyPrint() const {
  
  std::stringstream output;

  // Iterate through typeMap
    for (std::unordered_map<std::string, std::vector<InventoryItem>>::const_iterator
             it = typeMap_.begin(); it != typeMap_.end(); ++it) {
      std::string type = it->first;
      const std::vector<InventoryItem>& items = it->second;

      // Print header
      output << std::endl;
      if(type[type.size()-1] != 's')
        type += "s";
      output << "==============================  " << type
                << "  ==============================" << std::endl;

      // Print each item
      for (std::vector<InventoryItem>::const_iterator iter = items.begin();
        iter != items.end(); ++iter) {

        // Check for existing paint and cert to prevent uneven spacing
        std::string color = iter->GetColor();
        if (color != "Default")  // People omit "Default" or "Unpainted" when selling unpainted items
            output << "[" << color << "]" << " ";

        std::string cert = iter->GetCertification();
        if (cert != "")
            output << "[" << cert << "]" << " ";

        output << iter->GetName() << " (" << iter->GetQuantity() << ") "
        << iter->GetPriceRange() << "k" << std::endl; 
      }
    }

    return output.str();
}

// Returns a list of items with lower bound prices listed
std::string Inventory::PrintBuyingList() const {

    std::stringstream output;

    output << "                             "
              << "BUYING ITEMS"
              << "                               " << std::endl;

    // Iterate through typeMap
    for (std::unordered_map<std::string,
                            std::vector<InventoryItem>>::const_iterator it =
             typeMap_.begin();
         it != typeMap_.end(); ++it) {
        std::string type = it->first;
        const std::vector<InventoryItem>& items = it->second;

        // Print header
        output << std::endl;
        if (type[type.size() - 1] != 's') type += "s";
        output << "==============================  " << type
                  << "  ==============================" << std::endl;

        // Print each item (W = Want, H = Have, k = keys)
        for (std::vector<InventoryItem>::const_iterator iter = items.begin();
             iter != items.end(); ++iter) {

            output << " H: ";

            // A player cannot trade fractions of a key,
            // Therefore items priced under one key must be supplemented with an offer
            int keys = std::floor(iter->GetPriceLowerBound());
            if (keys == 0) 
              output << "Offer ";
            else 
              output << keys << "k";
             output << std::endl << "                               " << " W: ";
            
            // Check for existing paint and cert to prevent uneven spacing
            std::string color = iter->GetColor();
            if (color != "Default") // People omit "Default" or "Unpainted" when selling unpainted items
              output << "[" << color << "]" << " ";

            std::string cert = iter->GetCertification();
            if (cert != "") 
              output << "[" << cert << "]" << " ";

            output << iter->GetName() << std::endl;
        }
    }

    return output.str();
}

// Returns a list of items with upper bound prices listed
std::string Inventory::PrintSellingList() const {

    std::stringstream output;

    output << "                             "
              << "SELLING ITEMS"
              << "                               " << std::endl;

    // Iterate through typeMap
    for (std::unordered_map<std::string,
                            std::vector<InventoryItem>>::const_iterator it =
             typeMap_.begin();
         it != typeMap_.end(); ++it) {
        std::string type = it->first;
        const std::vector<InventoryItem>& items = it->second;

        // Print header
        output << std::endl;
        if (type[type.size() - 1] != 's') type += "s";
        output << "==============================  " << type
                  << "  ==============================" << std::endl;

        // Print each item (W = Want, H = Have, k = keys)
        for (std::vector<InventoryItem>::const_iterator iter = items.begin();
             iter != items.end(); ++iter) {
            output << " H: ";
            
            // Check for existing paint and cert to prevent uneven spacing
            std::string color = iter->GetColor();
            if (color != "Default")  // People omit "Default" or "Unpainted" when selling unpainted items
                output << "[" << color << "]" << " ";

            std::string cert = iter->GetCertification();
            if (cert != "")
                output << "[" << cert << "]"  << " ";
            
            output << iter->GetName()
                      << std::endl
                      << "                               " 
                      << " W: " << std::round(iter->GetPriceUpperBound()) << "k"
                      << " or Best Offer " << std::endl;
        }
    }

    return output.str();
}

// Returns an iterator to the passed item or the end if not found
std::vector<InventoryItem>::iterator Inventory::FindItem(const InventoryItem& item) {
    
    for (std::vector<InventoryItem>::iterator it = items_.begin();
         it != items_.end(); ++it) {
        if (*it == item) {
            return it;
        }
    }
    return items_.end();
}

// Return all items
std::vector<InventoryItem> Inventory::GetItems() {
  return items_;
}

// Creates a "saved" folder and stores an inventory for faster retrieval
bool Inventory::WriteInvToFile() {
  
  // Creates the "saved" folder
  std::filesystem::create_directory("..//saved//");

  // Creates a text file containing each inventory item
  std::ofstream output("..//saved//inventory.txt");

  // Write each inventory item to file to prevent reclassification
  for(const InventoryItem & item : items_) {
    output << item.GetName() << std::endl 
          << item.GetCertification() << std::endl 
          << item.GetColor() << std::endl 
          << item.GetPriceRange() << std::endl 
          << item.GetQuantity() << std::endl
          << item.IsTradable() << std::endl;
  }

  // Check that the file was created
  std::ifstream check("..//saved//inventory.txt");
  if (check)
    return true;
  return false;
}

// Reads in and populates an inventory from saved/inventory.txt
// Returns whether or not the process could be completed successfully
bool Inventory::ReadInvFromFile() {

  // Check for an existing saved inventory
  std::ifstream input("..//saved//inventory.txt");

  if (input) {

    // Clear and replace inventory
    items_.clear();

    std::string name, cert, color, price, qt, tradable;

    while(!input.eof()) {
      // Capture each trait
      std::getline(input,name);
      std::getline(input, cert);
      std::getline(input, color);
      std::getline(input, price);
      std::getline(input, qt);
      std::getline(input, tradable);
      
      // Exit if incomplete item or empty inventory
      if (qt.empty() || tradable.empty())
        break;

      int quantity = std::stoi(qt);
      bool canTrade = std::stoi(tradable);

      // Create an InventoryItem object and add it to the Inventory
      InventoryItem item(name, cert, color, canTrade, quantity, price);
      AddItem(item);
    }
    return true;
  }
  return false;
}