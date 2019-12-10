#pragma once

/* Rocket League Item Classifier
by Ridas Jagelavicius
*/

#include <string>
#include <opencv2/opencv.hpp>
#include "ItemDatabase.h"
#include "InventoryItem.h"

class ItemClassifier {
   public:
	// Custom constructor
    ItemClassifier(std::string full_path_to_model,
                   std::string path_to_database_json);

    // Detects all the boxes of text in an image
    void DetectText(std::string full_path_to_image);

    // Extracts text from boxes detected by DetectText()
    std::vector<std::string> ExtractText();

    // Attempts to match extracted text to a real item
    // Returns an empty string if unsuccessful
    // This method only strives to rearrange word orderings or filter out noise like [ or :
    std::string MatchTextToItemName(const std::vector<std::string>& words);

	// Extracts item paint color from extracted text
    std::string ExtractColor(std::vector<std::string>& extracted);

	// Extracts item certifications from extracted text
    std::string ExtractCertification(std::vector<std::string>& extracted);

	// Draws the base image with rendered text-detections
    void RenderTextDetections() const;
   private:
    std::string path_to_model_; // The path to the model used to detect text
    cv::Mat image_; // The raw image created in DetectText()
    ItemDatabase
        database_;  // The database used to match extracted text with an item
    std::vector<cv::RotatedRect>
        boxes_;  // The text-boxes populated by DetectText()
    std::vector<int>
        indices_;  // The indices of bounding boxes populated by DetectText()

	// Decode the positions and orientations of the text boxes
    // Ref:
    // https://github.com/spmallick/learnopencv/blob/master/TextDetectionEAST/textDetection.cpp
    void Decode(const cv::Mat& scores, const cv::Mat& geometry,
                float scoreThresh, std::vector<cv::RotatedRect>& detections,
                std::vector<float>& confidences);

	cv::Rect AddPadding(cv::Mat input_image, cv::Rect cropped_box, int padding); // Adds padding to text detections
    void Sanitize(std::string& word_or_item);  // Sanitizes words for better matching
    int CountNumberOfWords(const std::string& sanitized_string); // Counts the number of words in a sanitized string
    std::vector<std::string> SplitStringOnSpace(const std::string & sanitized_string); // Splits a string on space to create a vector of words
};