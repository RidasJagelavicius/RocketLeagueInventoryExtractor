#pragma once

/* Rocket League Item Price Database API
by Ridas Jagelavicius

Prices referenced from rl.insider.gg
*/

#include <string>
#include <json\json.h>

class ItemDatabase {
   public:
	  /** Default constuctor
        Mainly used to suppress warnings when initializing objects that have an ItemDatabase
    */
    ItemDatabase();

	  /** Custom constructor that takes in a file path to a JSON database
        @param path_to_database_json - The full file path to the JSON that will be used to create an ItemDatabase object
    */
    ItemDatabase(std::string path_to_database_json);

	  /** Returns the price of the "Default" color of an item, -1 if it does not exist, or -2 if the file path was invalid
        @param item_name - The name of the item to query the current price of
        @return The price of the Default color if successful, else -1 or -2
    */
	  std::string GetPriceOf(std::string item_name) const;

	  /** Returns the price of the color variant of an item, -1 if it does not exist, or -2 if the file path was invalid
        @param item_name - The name of the item to query the current price of
        @param color - The color variant of the item
        @return The price of the color variant if successful, else -1 or -2
    */
    std::string GetPriceOf(std::string item_name, std::string color) const;

	  /** Returns the rarity of an item
        @param item_name - The name of the item to query the rarity of
        @return The rarity of the item if valid, ex. Black Market, else -1 or -2
    */
    std::string GetRarityOf(std::string item_name) const;

	  /** Returns the type of an item
        @param item_name - The name of the item to query the type of
        @return The item's type if valid, ex. Decal, else -1 or -2
    */
    std::string GetTypeOf(std::string item_name) const;

	  /** Returns the full, pretty name of the item
        @param item_name - The name of the item to query the full name of
        @return The item's full name if valid ex. Octane - MG-88, else -1 or -2
    */
    std::string GetFullNameOf(std::string item_name) const;

	  /** Returns the name of all items in the database
        @return A vector of the full name of each item in the database
    */
	  std::vector<std::string> GetAllNames() const;

	  /** Returns whether the database link was valid
        @return Whether the path to the database JSON was valid and could be used to establish a database
    */
	  bool IsValidDatabase() const;

   private:
    Json::Value database_; // The actual database to query based on the filepath
    bool IsValidDatabase_; // Flag set upon pass of file path, checked in calls to GetPriceOf
    std::string Sanitize(std::string & input_string) const; // Sanitizes input to remove whitespace and convert to lowercase
};