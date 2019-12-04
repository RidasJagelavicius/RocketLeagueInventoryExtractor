/* Rocket League Item Price Database API
by Ridas Jagelavicius

Prices referenced from rl.insider.gg
*/

#include <string>
#include <fstream>
#include "ItemDatabase.h"

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
    if (!IsValidDatabase_) return "-2";
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
            std::string price = database_[item]["rarity"].asString();
            return price;
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
            std::string price = database_[item]["type"].asString();
            return price;
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
            std::string price = database_[item]["name"].asString();
            return price;
        } catch (const std::runtime_error &) {
            // This block is reached if the item does not exist
            return "-1";
        }
    }
}

// Sanitizes input to remove whitespace and convert to lowercase
std::string ItemDatabase::Sanitize(std::string &input_string) const { 
	std::stringstream sanitized; 
	transform(input_string.begin(), input_string.end(),
                  input_string.begin(), ::tolower);
        for (unsigned i = 0; i < input_string.size(); i++) {
            if (input_string[i] != ' ') sanitized << input_string[i];
        }
        return sanitized.str();
}

// Returns whether the database link was valid
bool ItemDatabase::IsValidDatabase() const { 
	// This is mainly just for testing purposes
	// But can help debug if calls to other functions return "-2"
	return IsValidDatabase_; }