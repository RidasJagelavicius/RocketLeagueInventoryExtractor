#pragma once

#include <string>

/* Rocket League Inventory Items API
by Ridas Jagelavicius */
class InventoryItem {
   public:
    /** Custom ctor
        @param name - The full name of the item ex. 20XX
    */
    InventoryItem(std::string name);

    /** Custom ctor
        @param name - The full name of the item ex. 20XX
        @param paint - The color of the item ex. Default or Burnt Sienna
    */
	  InventoryItem(std::string name,
                    std::string paint);

    /** Custom ctor
        @param name - The full name of the item ex. 20XX
        @param certification - The base certification of the item ex. Sniper or Aviator
        @param paint - The color of the item ex. Default or Burnt Sienna
    */
    InventoryItem(std::string name, std::string certification,
                    std::string paint);

    /** Custom ctor
        @param name - The full name of the item ex. 20XX
        @param certification - The base certification of the item ex. Sniper or Aviator
        @param paint - The color of the item ex. Default or Burnt Sienna
        @param price - The price range of the item ex. 40-50 or 2-3
    */
    InventoryItem(std::string name, std::string certification,
    std::string paint, std::string price);

    /** Custom ctor
        @param name - The full name of the item ex. 20XX
        @param certification - The base certification of the item ex. Sniper or Aviator
        @param paint - The color of the item ex. Default or Burnt Sienna
        @param tradable - Whether or not the item can be traded
        @param quantity - How many of the item are available 
    */
	  InventoryItem(std::string name, std::string certification,
                    std::string paint, bool tradable, int quantity);

    /** Custom ctor - Used for reading inventory from file
        @param name - The full name of the item ex. 20XX
        @param certification - The base certification of the item ex. Sniper or Aviator
        @param paint - The color of the item ex. Default or Burnt Sienna
        @param tradable - Whether or not the item can be traded
        @param quantity - How many of the item are available
        @param price - The price range of the item ex. 40-50 or 2-3
    */
    InventoryItem(std::string name, std::string certification,
                        std::string paint, bool tradable, int quantity, std::string price);
  
    /** Custom ctor
        @param name - The full name of the item ex. 20XX
        @param certification - The base certification of the item ex. Sniper or Aviator
        @param paint - The color of the item ex. Default or Burnt Sienna
        @param rarity - How rare the item is ex. Black Market or Rare
        @param tradable - Whether or not the item can be traded
        @param type - The item's category ex. Topper or Antenna
        @param quantity - How many of the item are available
        @param price - The price range of the item ex. 40-50 or 2-3
    */
    InventoryItem(std::string name, std::string certification,
                  std::string paint, std::string rarity, 
				  bool tradable, std::string type, int quantity, std::string price);

	  /** Returns the name of the item ex. "20XX"
        @return The item's name
    */
	  std::string GetName() const; 

	  /** Returns whether the item is certified
        @return Whether the item's certification is empty
    */
	  bool IsCertified() const;

	  /** Return the base certification of the item
      or an empty string if the item is not certified

      ex. An item certified as "Fantastic Striker" will return "Striker"
      @return The base certification of the item or an empty string
	  */
    std::string GetCertification() const;

	  /** Returns whether the item is painted
        @return Whether the item's paint color is empty
    */
    bool IsPainted() const;

	  /** Return the color of the item
        or an empty string if the item is not painted
        @return The item's color or an empty string
    */
    std::string GetColor() const;

	  /** Returns the rarity of the item ex. "Limited"
        @return The rarity of the item if specified upon initialization
    */
	  std::string GetRarity() const;

	  /** Returns whether an item is tradable
        @return Whether an item is tradable
    */
    bool IsTradable() const;

	  /** Return the type of the item ex. "Topper"
        @return The item's type if specified upon initialization
    */
    std::string GetType() const;

	  /** Return the quantity of the item
		  Modifications do not count towards the total.
		  If something went wrong (tried to update quantity to an impossible value)
			  then quantity will be -1

		  ex. If there is a certified wheel and an uncertified wheel,
		  the quantity will be 1 and 1, not 2.

      @return The amount of the item available (its quantity) or -1
	  */
	  int GetQuantity() const;

	  /** Update the quantity of the item, perhaps after buying or selling an item
    */
	  void UpdateQuantity(int new_quantity);

	  /** Returns the lower range of an item's price
        ex. An item priced 7-10 keys would return 7
                
        This could be useful for determining a BUY price 
        @return The lower bound of the provided price range
    */
    double GetPriceLowerBound() const;

    /** Returns the upper range of an item's price
        ex. An item priced 7-10 keys would return 10
            
        This could be useful for determining a SELL price 
        @return The upper bound of the provided price range
    */
    double GetPriceUpperBound() const;

    /** Returns the range of an item's price
        ex. An item priced 7-10 keys would return "7-10"
        @return The string representation of an item's initialized or updated price range  
    */
    std::string GetPriceRange() const;

	  /** Set the new price range of the item ex. UpdatePrice("2-5");
        @param price_range - The price range of the item ex. 40-50 or 2-3
    */
	  void UpdatePrice(std::string price_range);

    /** Determines whether two items have the same properties
        @return Whether two items have the same name, color, cert, type, and rarity
    */
    bool operator==(const InventoryItem& rhs) const;

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