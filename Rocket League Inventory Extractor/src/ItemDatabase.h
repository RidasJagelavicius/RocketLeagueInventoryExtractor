#pragma once

/* Rocket League Item Price Database API
by Ridas Jagelavicius

Prices referenced from rl.insider.gg
*/

#include <string>
#include <json\json.h>

class ItemDatabase {
   public:
	// Default constuctor - for suppessing warnings
    ItemDatabase();

	// Custom constructor that takes in a file path to a JSON database
    ItemDatabase(std::string path_to_database_json);

	// Returns the price of the "Default" color of an item, -1 if it does not exist, or -2 if the file path was invalid
	// Note: Truncate Spaces
	std::string GetPriceOf(std::string item_name) const;

	// Returns the price of the color variant of an item, -1 if it does not exist, or -2 if the file path was invalid
    std::string GetPriceOf(std::string item_name, std::string color) const;

	// Returns the rarity of an item
    std::string GetRarityOf(std::string item_name) const;

	// Returns the type of an item
    std::string GetTypeOf(std::string item_name) const;

	// Returns the full, pretty name of the item
    std::string GetFullNameOf(std::string item_name) const;

	// Returns the name of all items in the database
	std::vector<std::string> & GetAllNames() const;

	// Returns whether the database link was valid 
	bool IsValidDatabase() const;

   private:
    Json::Value database_; // The actual database to query based on the filepath
    bool IsValidDatabase_; // Flag set upon pass of file path, checked in calls to GetPriceOf
    std::string Sanitize(std::string & input_string) const; // Sanitizes input to remove whitespace and convert to lowercase
};