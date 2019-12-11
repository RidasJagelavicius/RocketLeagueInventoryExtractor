#pragma once

#include <string>

/* Rocket League Inventory Items API
by Ridas Jagelavicius */
class InventoryItem {
   public:
	// Custom ctors for creating an inventory item
    InventoryItem(std::string name);

	InventoryItem(std::string name,
                  std::string paint);

    InventoryItem(std::string name, std::string certification,
                  std::string paint);

  InventoryItem(std::string name, std::string certification,
  std::string paint, std::string price);

	InventoryItem(std::string name, std::string certification,
                  std::string paint, bool tradable, int quantity);

    InventoryItem(std::string name, std::string certification,
                  std::string paint, std::string rarity, 
				  bool tradable, std::string type, int quantity, std::string price);

	// Returns the name of the item ex. "20XX"
	std::string GetName() const; 

	// Returns whether the item is certified
	bool IsCertified() const;

	/* Return the base certification of the item
    or an empty string if the item is not certified

    ex. An item certified as "Fantastic Striker" will return "Striker"
	*/
    std::string GetCertification() const;

	// Returns whether the item is painted
    bool IsPainted() const;

	/* Return the color of the item
            or an empty string if the item is not painted
    */
    std::string GetColor() const;

	// Returns the rarity of the item ex. "Limited"
	std::string GetRarity() const;

	// Returns whether an item is tradable
    bool IsTradable() const;

	// Return the type of the item ex. "Topper"
    std::string GetType() const;

	/* Return the quantity of the item
		Modifications do not count towards the total.
		If something went wrong (tried to update quantity to an impossible value)
			then quantity will be -1

		ex. If there is a certified wheel and an uncertified wheel,
		the quantity will be 1 and 1, not 2.
	*/
	int GetQuantity() const;

	// Update the quantity of the item, perhaps after buying or selling an item
	void UpdateQuantity(int new_quantity);

	/* Returns the lower range of an item's price
            ex. An item priced 7-10 keys would return 7
                
            This could be useful for determining a BUY price */
    double GetPriceLowerBound() const;

    /* Returns the upper range of an item's price
        ex. An item priced 7-10 keys would return 10
            
        This could be useful for determining a SELL price */
    double GetPriceUpperBound() const;

    /* Returns the range of an item's price
    ex. An item priced 7-10 keys would return "7-10"
        
    This could be useful for determining a SELL price */
    std::string GetPriceRange() const;

	// Set the new price range of the item ex. UpdatePrice("2-5");
	void UpdatePrice(std::string price_range);

	private:
		 std::string name_; // Name of the item ex. "20XX"
         std::string certification_; // Base certification of the item ex. "Striker"
         std::string paint_; // Paint color of the item ex. "Pink"
         std::string rarity_; // Rarity of the item ex. "Limited" or "Rare"
         bool tradable_; // Whether the item is actually tradable
         std::string type_; // The type of the item ex. "Topper" or "Wheels"
         int quantity_; // The quantity of the unique item so far. A certified and uncertified item do not increase the total quantity
         std::string price_; // The price range of the item in keys ex. "7-10"
};