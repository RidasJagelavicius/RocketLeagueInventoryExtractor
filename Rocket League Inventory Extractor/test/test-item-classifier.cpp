#include <string>

#include "../catch.hpp"
#include "../src/ItemClassifier.h"
#include "../src/ItemDatabase.h"

ItemClassifier classifier(
    "C:\\Users\\Unknown_User\\Documents\\openFrameworks\\apps\\fantastic-"
    "finale-astudent82828211\\Rocket League Inventory "
    "Extractor\\frozen_east_text_detection.pb",
    "C:\\Users\\Unknown_User\\Documents\\openFrameworks\\apps\\fantastic-"
    "finale-astudent82828211\\Rocket League Inventory Extractor\\Prices.json");

TEST_CASE("ItemClassifier extracts text") {
    std::string path_to_folder =
        "C:\\Users\\Unknown_User\\Documents\\openFrameworks\\apps\\fantastic-"
        "finale-astudent82828211\\Rocket League Inventory Extractor\\Test "
        "Images for RL\\Isolated\\";

	std::string image = "CobaltWildcatEars.png";
    classifier.DetectText(path_to_folder + image);
    std::vector<std::string> extracted = classifier.ExtractText();
    REQUIRE(!extracted.empty());
}

TEST_CASE("ExtractColor successfully extracts and removes paints") {
    std::vector<std::string> extracted = {"wildcat", "COBALT", "ears"};
    std::string color = classifier.ExtractColor(extracted);
    REQUIRE(color == "Cobalt");
    REQUIRE(extracted.size() == 2);
}

TEST_CASE("ExtractColor successfully extracts and removes two-word paints") {
    std::vector<std::string> extracted = {"wildcat", "BURNT", "SIENNA", "ears"};
    std::string color = classifier.ExtractColor(extracted);
    REQUIRE(color == "Burnt Sienna");
    REQUIRE(extracted.size() == 2);
}

TEST_CASE("ExtractColor returns Default if unpainted item") {
    std::vector<std::string> extracted = {"wildcat", "ears"};
    std::string color = classifier.ExtractColor(extracted);
    REQUIRE(color == "Default");
    REQUIRE(extracted.size() == 2);
}

TEST_CASE("ExtractCertification extracts and removes certifications") {
    std::vector<std::string> extracted = {"wildcat", "ears", "paragon"};
    std::string cert = classifier.ExtractCertification(extracted);
    REQUIRE(cert == "Paragon");
    REQUIRE(extracted.size() == 2);
}

TEST_CASE("ExtractCertification extracts and removes certifications and modifiers") {
    std::vector<std::string> extracted = {"wildcat","capable", "ears", "paragon"};
    std::string cert = classifier.ExtractCertification(extracted);
    REQUIRE(cert == "Paragon");
    REQUIRE(extracted.size() == 2);
}

TEST_CASE("ExtractCertification returns an empty string if uncertified item") {
    std::vector<std::string> extracted = {"wildcat", "ears",};
    std::string cert = classifier.ExtractCertification(extracted);
    REQUIRE(cert == "");
    REQUIRE(extracted.size() == 2);
}

TEST_CASE("MatchTextToItemName works with a simple word") { 
    std::vector<std::string> words;
    std::string word = "spiralis";
    words.push_back(word);
    std::string solution = "Spiralis";
    REQUIRE(solution == classifier.MatchTextToItemName(words)); 
}

TEST_CASE("MatchTextToItemName works with two words") {
    std::vector<std::string> words;
    std::string word1 = "Animus:";
    std::string word2 = "GP";
    words.push_back(word1);
    words.push_back(word2);
    std::string solution = "Animus GP";
    REQUIRE(solution == classifier.MatchTextToItemName(words));
}

TEST_CASE("MatchTextToItemName works with two words in reversed order #1") {
    std::vector<std::string> words;
    std::string word1 = "Animus";
    std::string word2 = "GP";
    words.push_back(word2);
    words.push_back(word1);
    std::string solution = "Animus GP";
    REQUIRE(solution == classifier.MatchTextToItemName(words));
}

TEST_CASE("MatchTextToItemName works with two words in reversed order #2") {
    std::vector<std::string> words;
    std::string word1 = "MG-88";
    std::string word2 = "Octane:";
    words.push_back(word1);
    words.push_back(word2);
    std::string solution = "Octane - MG-88";
    REQUIRE(solution == classifier.MatchTextToItemName(words));
}

TEST_CASE("MatchTextToItemName returns an empty string if a match couldn't be made") {
    std::vector<std::string> words;
    std::string word1 = "Shouldnt";
    std::string word2 = "Match";
    words.push_back(word2);
    words.push_back(word1);
    std::string solution = "";
    REQUIRE(solution == classifier.MatchTextToItemName(words));
}