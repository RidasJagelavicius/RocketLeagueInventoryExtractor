/* Rocket League Item Price Database API
by Ridas Jagelavicius

Prices referenced from rl.insider.gg
*/

#include <string>
#include <fstream>
#include <iostream>
#include "ItemDatabase.h"

// Default constuctor - for suppessing warnings
ItemDatabase::ItemDatabase() { 
	IsValidDatabase_ = false; 
}

// Custom constructor that takes in a file path to a JSON database
ItemDatabase::ItemDatabase(std::string path_to_database_json) {
    std::ifstream input(path_to_database_json);
    if (input) {
		// File is valid
        IsValidDatabase_ = true;

		// Attach the filepath to the database
        Json::Reader reader;
        reader.parse(input, database_);
    } else {
		// File is invalid, set flag
        IsValidDatabase_ = false;
	}
}

// Returns the price of the default color of an item or -1 if it does not exist, or -2 if the file path was invalid
std::string ItemDatabase::GetPriceOf(std::string item_name) const {
    if (!IsValidDatabase_) 
        return "-2";
    else {
        std::string item = ItemDatabase::Sanitize(item_name);

        // Get price of object
        std::string price = database_[item]["prices"]["default"].asString();

		// Sometimes an item does not have a "Default" color variant, in which case it shows up as ""
        if (price.empty()) return "-1";
        return price;
	}
}

// Returns the price of the color variant of an item or -1 if it does not exist, or -2 if the file path was invalid
std::string ItemDatabase::GetPriceOf(std::string item_name,
	std::string color) const {
    if (!IsValidDatabase_)
        return "-2";
    else {
        std::string item = ItemDatabase::Sanitize(item_name);
        std::string col = ItemDatabase::Sanitize(color);

		try {
            // Get price of object
            std::string price = database_[item]["prices"][col].asString();
            if (price.empty()) {
				// Indicates that there is no variant of that color
                return "-1";
			}
            return price;
        } catch (const std::runtime_error &) {
			// This block is reached if the color variant of the item does not exist
            return "-1";
		}
    }
}

// Returns the rarity of an item
std::string ItemDatabase::GetRarityOf(std::string item_name) const {
    if (!IsValidDatabase_)
        return "-2";
    else {
        std::string item = ItemDatabase::Sanitize(item_name);

        try {
            // Get rarity of object
            std::string rarity = database_[item]["rarity"].asString();
            return rarity;
        } catch (const std::runtime_error &) {
            // This block is reached if the item does not exist
            return "-1";
        }
    }
}

// Returns the type of an item
std::string ItemDatabase::GetTypeOf(std::string item_name) const {
    if (!IsValidDatabase_)
        return "-2";
    else {
        std::string item = ItemDatabase::Sanitize(item_name);

        try {
            // Get type of object
            std::string type = database_[item]["type"].asString();
            return type;
        } catch (const std::runtime_error &) {
            // This block is reached if the item does not exist
            return "-1";
        }
    }
}

// Returns the full, pretty name of the item
std::string ItemDatabase::GetFullNameOf(std::string item_name) const {
    if (!IsValidDatabase_)
        return "-2";
    else {
        std::string item = ItemDatabase::Sanitize(item_name);

        try {
            // Get type of object
            std::string name = database_[item]["name"].asString();

            if (name.empty()) return "-1";
            return name;
        } catch (const std::runtime_error &) {
            // This block is reached if the item does not exist
            return "-1";
        }
    }
}

// Sanitizes input to remove whitespace and convert to lowercase
std::string ItemDatabase::Sanitize(std::string &input_string) const { 

  // Copy input string
    std::string word_or_item = input_string;

	// Define strange symbols to be removed
    std::vector<char> toRemove = {'\n', '-', ' ' ,'[', ']', '{', '}', '!',
                                  '/',  '\\', '.', ':', ';', '_'};

    bool wasErased;
    // Loop through word or item
    std::string::iterator it = word_or_item.begin();
    while (it != word_or_item.end()) {
        wasErased = false;

        // Remove any strange characters
        for (std::vector<char>::iterator it2 = toRemove.begin();
             it2 != toRemove.end(); ++it2) {
            if (*it == *it2) {
                it = word_or_item.erase(it);
                wasErased = true;
                break;
            }
        }

        if (it != word_or_item.end() && !wasErased) ++it;
    }

    // Convert word to lowercase
    std::transform(word_or_item.begin(), word_or_item.end(),
                   word_or_item.begin(), ::tolower);

    return word_or_item;
}

// Returns the name of all items in the database
std::vector<std::string> ItemDatabase::GetAllNames() const {
    std::vector<std::string> names;
    if (IsValidDatabase_) {

		// Push the full name of every item
        for (Json::ValueConstIterator it = database_.begin(); it != database_.end() && it.name() != "0"; ++it) {
            names.push_back(GetFullNameOf(it.name()));
        }
	}
    return names;
}

// Returns whether the database link was valid
bool ItemDatabase::IsValidDatabase() const { 
	// This is mainly just for testing purposes
	// But can help debug if calls to other functions return "-2"
	return IsValidDatabase_; }