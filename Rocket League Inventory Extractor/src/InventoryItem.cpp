/* Rocket League Inventory Item API
by Ridas Jagelavicius */

#include <string>
#include <sstream>
#include <iostream>
#include "InventoryItem.h"

InventoryItem::InventoryItem(std::string name)
    : name_(name),
      certification_(""),
      paint_(""),
      rarity_(""),
      tradable_(""),
      type_(""),
      quantity_(1),
      price_("") {
        /* Nothing */ }

InventoryItem::InventoryItem(std::string name, std::string paint)
    : name_(name),
      certification_(""),
      paint_(paint),
      rarity_(""),
      tradable_(""),
      type_(""),
      quantity_(1),
      price_("") {
        /* Nothing */ }

InventoryItem::InventoryItem(std::string name, std::string certification,
                             std::string paint)
    : name_(name),
      certification_(certification),
      paint_(paint),
      rarity_(""),
      tradable_(""),
      type_(""),
      quantity_(1),
      price_("") {
        /* Nothing */ }

InventoryItem::InventoryItem(std::string name, std::string certification,
                             std::string paint, std::string price)
    : name_(name),
      certification_(certification),
      paint_(paint),
      rarity_(""),
      tradable_(""),
      type_(""),
      quantity_(1),
      price_(price) {
        /* Nothing */ }

InventoryItem::InventoryItem(std::string name, std::string certification,
                             std::string paint,
              bool tradable, int quantity)
    : name_(name),
      certification_(certification),
      paint_(paint),
      rarity_(""),
      tradable_(tradable),
      type_(""),
      quantity_(quantity),
      price_("") {
        /* Nothing */ }

InventoryItem::InventoryItem(std::string name, std::string certification,
                           std::string paint,
              std::string rarity, bool tradable, std::string type, int quantity,
              std::string price)
    : name_(name),
      certification_(certification),
      paint_(paint),
      rarity_(rarity),
      tradable_(tradable),
      type_(type),
      quantity_(quantity),
      price_(price) { 
	/* Nothing */ }

// Returns the name of the item ex. "20XX"
std::string InventoryItem::GetName() const { 
	return name_; }

// Returns whether the item is certified
bool InventoryItem::IsCertified() const {
	return !certification_.empty(); }

/* Return the base certification of the item
or an empty string if the item is not certified
*/
std::string InventoryItem::GetCertification() const { 
	return certification_; }

// Returns whether the item is painted
bool InventoryItem::IsPainted() const { 
	return !paint_.empty(); }

/* Return the color of the item
    or an empty string if the item is not painted
*/
std::string InventoryItem::GetColor() const { 
	return paint_; }

// Returns the rarity of the item ex. "Limited"
std::string InventoryItem::GetRarity() const { 
	return rarity_; }

// Returns whether an item is tradable
bool InventoryItem::IsTradable() const {  
	// An item should also not be traded if no prices have been reported for it yet
	// A valid price is at least length 3 ("3-4" keys for example)
    return tradable_; }

// Return the type of the item ex. "Topper"
std::string InventoryItem::GetType() const { 
	return type_; }

// Return the quantity of the item
int InventoryItem::GetQuantity() const { 
	return quantity_; }

// Update the quantity of the item, perhaps after buying or selling an item
void InventoryItem::UpdateQuantity(int new_quantity) {
    // Have error bounds checking on quantity
    if (new_quantity < 0)
        quantity_ = -1;
    else
        quantity_ = new_quantity;
}

// Returns the lower range of an item's price
double InventoryItem::GetPriceLowerBound() const {
    std::stringstream lower_bound;

    // Loop through price and stop at hyphen
    for (unsigned i = 0; i < price_.size(); i++) {
        if (price_[i] == '-')
            break;
        else
            lower_bound << price_[i];
    }

	// Convert string to double
    return std::stod(lower_bound.str());
}

// Returns the lower range of an item's price
double InventoryItem::GetPriceUpperBound() const {
    std::stringstream upper_bound;
    bool is_upper = false;

    // Loop through price and start at hyphen
    for (unsigned i = 0; i < price_.size(); i++) {
        if (!is_upper && price_[i] == '-')
            is_upper = true;
        else if (is_upper)
            upper_bound << price_[i];
    }

    // Convert string to double
    return std::stod(upper_bound.str());
}

// Returns the range of an item's price
std::string InventoryItem::GetPriceRange() const { 
	return price_; }

// Set the new price range of the item ex. UpdatePrice("2-5");
void InventoryItem::UpdatePrice(std::string price_range) {
    price_ = price_range;
}

// Determines whether two items have the same properties
bool InventoryItem::operator==(const InventoryItem& rhs) const {
    return name_ == rhs.GetName() && certification_ == rhs.GetCertification() 
    && paint_ == rhs.GetColor() && rarity_ == rhs.GetRarity() && 
    tradable_ == rhs.IsTradable() && type_ == rhs.GetType();
}