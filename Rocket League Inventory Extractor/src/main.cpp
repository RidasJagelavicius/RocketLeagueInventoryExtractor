/* Rocket League Inventory Extractor API 
  Author: Ridas Jagelavicius */

#include <string>

#include "ItemClassifier.h"
#include "ItemDatabase.h"
#include "Inventory.h"

int main() {

 // Paths to initiate Item Classifier
 std::string path_to_model_for_text_detection = "C:\\Users\\Unknown_User\\Documents\\openFrameworks\\apps\\fantastic-finale-astudent82828211\\Rocket League Inventory Extractor\\frozen_east_text_detection.pb";
 std::string path_to_database =  "C:\\Users\\Unknown_User\\Documents\\openFrameworks\\apps\\fantastic-finale-astudent82828211\\Rocket League Inventory Extractor\\Prices.json";

 // Path to folder that holds image names
 std::string path_to_folder =  "C:\\Users\\Unknown_User\\Documents\\openFrameworks\\apps\\fantastic-finale-astudent82828211\\Rocket League Inventory Extractor\\Test Images for RL\\Isolated\\";

 // Load images to vector
 std::vector<std::string> tests;
 std::string t1 = path_to_folder + "BSFGSP.png";
 std::string t2 = path_to_folder + "CobaltWildcatEars.png";
 std::string t3 = path_to_folder + "Dominus.png";
 std::string t4 = path_to_folder + "OctaneMG88.png";
 std::string t5 = path_to_folder + "SaffSpiralis.png";
 tests.push_back(t1);
 tests.push_back(t2);
 tests.push_back(t3);
 tests.push_back(t4);
 tests.push_back(t5);

 ItemClassifier classifier(path_to_model_for_text_detection, path_to_database); // Extracts item info from image
 Inventory inv = Inventory(path_to_database); // Holds items
 ItemDatabase db(path_to_database); // Can be queried for prices, types, rarity, full names

 int imageNumber = 1;

 // Display each item and add it to inventory
 for (std::string& string : tests) {

     // Print each text extraction and show each text detection
     classifier.DetectText(string);
     std::vector<std::string> extracted = classifier.ExtractText();
     std::cout << std::endl << imageNumber++ << std::endl;
     for (const std::string& ex : extracted) {
         std::cout << ex;
	   }
     std::cout << std::endl;

     // Create an object of each text extraction and load into inventory
     std::string certification = classifier.ExtractCertification(extracted);
     std::string paint = classifier.ExtractColor(extracted);
     std::string name = classifier.MatchTextToItemName(extracted);
     std::string price = db.GetPriceOf(name,paint);

     std::cout << "Extracted Certification: " << certification << std::endl;
     std::cout << "Extracted Color: " << paint << std::endl;
     std::cout << "Matched Name: " << name << std::endl;
     std::cout << "Queried Price: " << price << std::endl;

     InventoryItem item(name, certification, paint, price);
     inv.AddItem(item);

     // Show detected text - Press any key to continue
     classifier.RenderTextDetections();
 }

 // Print different lists
 std::cout << inv.PrettyPrint() << std::endl;
 std::cout << inv.PrintSellingList() << std::endl;
 std::cout << inv.PrintBuyingList() << std::endl;

 // Saves the inventory to a file to prevent reclassification
 // To create an inventory based on the saved file,
 // Simply create an Inventory object and call Inventory::ReadInvFromFile()
 inv.WriteInvToFile();

 return 0;
}