#include <vector>
#include <string>
#include <cmath>
#include <iostream>

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

// Prints out an easy-to-read list of items
void Inventory::PrettyPrint() const {
  
  // Iterate through typeMap
    for (std::unordered_map<std::string, std::vector<InventoryItem>>::const_iterator
             it = typeMap_.begin(); it != typeMap_.end(); ++it) {
      std::string type = it->first;
      const std::vector<InventoryItem>& items = it->second;

      // Print header
      std::string category = type + "s";
      std::cout << "==============================  " << category << "  ==============================" << std::endl;

      // Print each item
      for (std::vector<InventoryItem>::const_iterator iter = items.begin();
        iter != items.end(); ++iter) {
        
        std::cout << " - " << iter->GetColor() << " " << iter->GetCertification() 
        << " " << iter->GetName() << " (" << iter->GetQuantity() << ") "
        << iter->GetPriceRange() << std::endl; 
      }
    }
}

// Prints out a list of items with lower bound prices listed
void Inventory::PrintBuyingList() const {

    // Iterate through typeMap
    for (std::unordered_map<std::string,
                            std::vector<InventoryItem>>::const_iterator it =
             typeMap_.begin();
         it != typeMap_.end(); ++it) {
        std::string type = it->first;
        const std::vector<InventoryItem>& items = it->second;

        // Print header
        std::string category = type + "s";
        std::cout << "==============================  " << category
                  << "  ==============================" << std::endl;

        // Print each item (W = Want, H = Have, k = keys)
        for (std::vector<InventoryItem>::const_iterator iter = items.begin();
             iter != items.end(); ++iter) {
            std::cout << " W: " << iter->GetColor() << " "
                      << iter->GetCertification() << " " << iter->GetName()
                      << std::endl << "                               " 
                      << " H: " << std::floor(iter->GetPriceLowerBound()) << "k" << std::endl;
        }
    }
}

// Prints out a list of items with upper bound prices listed
void Inventory::PrintSellingList() const {

    // Iterate through typeMap
    for (std::unordered_map<std::string,
                            std::vector<InventoryItem>>::const_iterator it =
             typeMap_.begin();
         it != typeMap_.end(); ++it) {
        std::string type = it->first;
        const std::vector<InventoryItem>& items = it->second;

        // Print header
        std::string category = type + "s";
        std::cout << "==============================  " << category
                  << "  ==============================" << std::endl;

        // Print each item (W = Want, H = Have, k = keys)
        for (std::vector<InventoryItem>::const_iterator iter = items.begin();
             iter != items.end(); ++iter) {
            std::cout << " H: " << iter->GetColor() << " "
                      << iter->GetCertification() << " " << iter->GetName()
                      << std::endl
                      << "                               " 
                      << " W: " << std::round(iter->GetPriceUpperBound()) << "k"
                      << " or offer " << std::endl;
        }
    }
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