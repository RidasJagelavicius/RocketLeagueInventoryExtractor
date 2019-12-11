/* Rocket League Item Classifier
by Ridas Jagelavicius
*/

#include <opencv2/core/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <string>
#include <fstream>

#include "ItemClassifier.h"
#include "ItemDatabase.h"

constexpr int HEIGHT = 320;         // Resizes preprocessed image to this height
constexpr int WIDTH = 320;          // Resizes preprocessed image to this width
constexpr float IMAGE_SCALE = 1.0;  // Preprocessing image scale factor
constexpr float CONFIDENCE_THRESHOLD = .50;  // How confident we want to be that our text box properly encloses the text
constexpr float NON_MAX_SUPPRESSION_THRESHOLD = .4;  // This will change detection accuracy and the number of text boxes made

// Custom constructor
ItemClassifier::ItemClassifier(std::string full_path_to_model,
                               std::string path_to_database_json) {
    std::ifstream model(full_path_to_model);

	// Load the model 
    if (model.good()) {
        path_to_model_ = full_path_to_model;
        std::cout << "Model found" << std::endl;
    } else {
        std::cout << "Model not found at provided path" << std::endl;
	}
    database_ = ItemDatabase::ItemDatabase(path_to_database_json);
}




// Decodes positions of text boxes
void ItemClassifier::Decode(const cv::Mat& scores, const cv::Mat& geometry,
                float scoreThresh, std::vector<cv::RotatedRect>& detections,
                std::vector<float>& confidences) {
    detections.clear();
    CV_Assert(scores.dims == 4);
    CV_Assert(geometry.dims == 4);
    CV_Assert(scores.size[0] == 1);
    CV_Assert(geometry.size[0] == 1);
    CV_Assert(scores.size[1] == 1);
    CV_Assert(geometry.size[1] == 5);
    CV_Assert(scores.size[2] == geometry.size[2]);
    CV_Assert(scores.size[3] == geometry.size[3]);

    const int height = scores.size[2];
    const int width = scores.size[3];
    for (int y = 0; y < height; ++y) {
        const float* scoresData = scores.ptr<float>(0, 0, y);
        const float* x0_data = geometry.ptr<float>(0, 0, y);
        const float* x1_data = geometry.ptr<float>(0, 1, y);
        const float* x2_data = geometry.ptr<float>(0, 2, y);
        const float* x3_data = geometry.ptr<float>(0, 3, y);
        const float* anglesData = geometry.ptr<float>(0, 4, y);
        for (int x = 0; x < width; ++x) {
            float score = scoresData[x];
            if (score < scoreThresh) continue;

            // Decode a prediction.
            // Multiple by 4 because feature maps are 4 time less than input
            // image.
            float offsetX = x * 4.0f, offsetY = y * 4.0f;
            float angle = anglesData[x];
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);
            float h = x0_data[x] + x2_data[x];
            float w = x1_data[x] + x3_data[x];

            cv::Point2f offset(
                offsetX + cosA * x1_data[x] + sinA * x2_data[x],
                offsetY - sinA * x1_data[x] + cosA * x2_data[x]);
            cv::Point2f p1 = cv::Point2f(-sinA * h, -cosA * h) + offset;
            cv::Point2f p3 = cv::Point2f(-cosA * w, sinA * w) + offset;
            cv::RotatedRect r(0.5f * (p1 + p3), cv::Size2f(w, h),
                                -angle * 180.0f / (float)CV_PI);
            detections.push_back(r);
            confidences.push_back(score);
        }
    }
}




 // Adds padding to text boxes for better text extraction
cv::Rect ItemClassifier::AddPadding(cv::Mat input_image, cv::Rect cropped_box,
                                    int padding) {
    cv::Rect returnRect = cv::Rect(
         cropped_box.x - padding, cropped_box.y - padding,
         cropped_box.width + (padding * 2), cropped_box.height + (padding * 2));
    if (returnRect.x < 0) 
		returnRect.x = 0;
    if (returnRect.y < 0) 
		returnRect.y = 0;
    if (returnRect.x + returnRect.width >= input_image.cols)
        returnRect.width = input_image.cols - returnRect.x;
    if (returnRect.y + returnRect.height >= input_image.rows)
        returnRect.height = input_image.rows - returnRect.y;
    return returnRect;
 }




// Detects all the boxes of text in an image
 void ItemClassifier::DetectText(std::string full_path_to_image) {
     /* Note:
         Tesseract is a popular text recognition model that maps an image of
    text to the actual content text. tesseract requires a bounded region
    within which to search for text. east text detection will detect and
    bound the region containing text. tesseract can then search within that
    region for text.
 */

     /* References:
     https://docs.opencv.org/master/db/da4/samples_2dnn_2text_detection_8cpp-example.html
     */

     /* ======================================= text-detection
      * =======================================*/

     cv::String model = path_to_model_;
     CV_Assert(!model.empty());

     // Load network.
     cv::dnn::Net net = cv::dnn::readNet(model);

     // Specify the output layers for the network
     std::vector<cv::Mat> outs;
     std::vector<cv::String> outNames(2);
     outNames[0] = "feature_fusion/Conv_7/Sigmoid";  // Geometry
     outNames[1] = "feature_fusion/concat_3";        // Confidence

     cv::Mat blob; // Initialize input image and processing image

     // Load in a test image
     image_ = cv::imread(full_path_to_image);

	 // Test that image was properly loaded
     if (image_.empty()) {
         std::cout << "Image not found at provided path" << std::endl;
         return;
	 }

     /* Preprocesses an image.
           The link below explains exactly how this works, but essentially,
           preprocessing is a multi-step process that "helps combat illumination
       changes"
       https://www.pyimagesearch.com/2017/11/06/deep-learning-opencvs-blobfromimage-works/
       */
         cv::dnn::blobFromImage(image_, blob, 1.0, cv::Size(WIDTH, HEIGHT),
                                cv::mean(image_),
                   true, false);

     // Pass the input image through the network and obtain geometry and
     // confidence scores
     net.setInput(blob);
     net.forward(outs, outNames);
     cv::Mat scores = outs[0];
     cv::Mat geometry = outs[1];

     // Decode predicted bounding boxes.
     std::vector<float> confidences;
     Decode(scores, geometry, CONFIDENCE_THRESHOLD, boxes_, confidences);

     // Filter out the best candidates for the correct text box using
     // non-maximum suppression
     cv::dnn::NMSBoxes(boxes_, confidences, CONFIDENCE_THRESHOLD,
                       NON_MAX_SUPPRESSION_THRESHOLD, indices_);
 }




// Extracts text from boxes detected by DetectText()
 std::vector<std::string> ItemClassifier::ExtractText() {
     std::vector<std::string> extracted;

     /* Reference for setup and implementation:
    https://stackoverflow.com/questions/18180824/how-to-implement-tesseract-to-run-with-project-in-visual-studio-2010
    */

     if (!image_.empty()) {

		 // Initialize OCR
         tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
         ocr->Init(NULL, "eng",
                   tesseract::OEM_DEFAULT);  // Set OCR to use English
         ocr->SetPageSegMode(static_cast<tesseract::PageSegMode>(
             8));  // Set OCR to read a single word

         cv::Point2f ratio((float)image_.cols / WIDTH,
                           (float)image_.rows / HEIGHT);

		 // Read the text of each detected box
         for (size_t i = 0; i < indices_.size(); ++i) {
             cv::RotatedRect& box = boxes_[indices_[i]];
             cv::Point2f vertices[4];
             box.points(vertices);

             for (int j = 0; j < 4; ++j) {
                 vertices[j].x *= ratio.x;
                 vertices[j].y *= ratio.y;
             }
             std::vector<cv::Point2f> points;

             for (const cv::Point2f& point : vertices) {
                 points.push_back(point);
             }

             cv::RotatedRect rect = cv::minAreaRect(
                 points);  // Creates a rectangle bounded to each text box
             cv::Rect rectangle =
                 rect.boundingRect2f();  // Makes the rectangle unrotated
             rectangle = ItemClassifier::AddPadding(
                 image_, rectangle, 2);  // Adds padding to the rectangle for better accuracy

             // Crop original image
             cv::Rect bounds(0, 0, WIDTH, HEIGHT);
             cv::Mat cropped = image_(rectangle & bounds);

			 // Extract text from crop
             ocr->SetImage(cropped.data, cropped.cols, cropped.rows, 3,
                           cropped.step);
             std::string text = std::string(ocr->GetUTF8Text());
             extracted.push_back(text);
         }
         delete ocr;
     } else {
         std::cout << "Image must be initialized before text can be extracted"
                   << std::endl;
	 }
	 return extracted;
 }




// Attempts to match extracted text to a real item
 std::string ItemClassifier::MatchTextToItemName(
     const std::vector<std::string>& words) { 

	 if (database_.IsValidDatabase()) {

		// Test if raw input matches with an item name
    std::stringstream ssWord;
    std::vector<std::string> sanitized_words;

    for (unsigned i = 0; i < words.size(); i++) {
      std::string w = words[i];
      Sanitize(w);
      ssWord << w;
      sanitized_words.push_back(w);

      if(i != words.size()-1) 
        ssWord << " ";
    }

    std::string dbName = database_.GetFullNameOf(ssWord.str());
    if (dbName != "-1" && dbName != "-2")
        return dbName;
    else { 
       // Attempt to match the extracted text to a real item
       std::vector<std::string>& allNames = database_.GetAllNames();
       int length = words.size();

       // Loop through all items and look for a good match
       for (std::string& itemName : allNames) {

           // Check for matching name length
           Sanitize(itemName);
           int numberOfWords = CountNumberOfWords(itemName);

           if (numberOfWords == length) {

            // Count the number of words in the possibleMatch that are in the passed item
            std::vector<std::string> possibleMatch = SplitStringOnSpace(itemName);
            int wordsInCommon = 0;

            for (const std::string& word : sanitized_words) {
                
                for (std::string& wordToMatch : possibleMatch) {
                    if (word == wordToMatch) {
                        wordsInCommon++;
                    }
                }

                // If each word matches, the word has been found
                if (wordsInCommon == length)
                  return database_.GetFullNameOf(itemName);
            }
           } else {
             continue;
           }
       }
    }
	 }
   return "";
 }

// Extracts item paint color from extracted text
 std::string ItemClassifier::ExtractColor(std::vector<std::string>& extracted) {
     std::string color;
     bool wasErased; // Prevent skipping an element after erasing

     // All available colors besides default
     std::vector<std::string> colors = {
         "black",  "white",   "grey", "crimson", "pink",   "cobalt", "blue",
         "sienna", "saffron", "lime", "green",   "orange", "purple"};

     // Some color modifiers
     std::vector<std::string> mods = {"burnt", "forest", "sky"};

     // Search extracted text for color
     std::vector<std::string>::iterator it = extracted.begin();
     while (it != extracted.end()) {
         std::string proposed_color = *it;
         wasErased = false;

         // Convert to lower case
         std::transform(proposed_color.begin(), proposed_color.end(),
                        proposed_color.begin(), ::tolower);

         // Loop through available colors and search for match
         for (std::vector<std::string>::iterator it2 = colors.begin();
              it2 != colors.end(); ++it2) {
             if (proposed_color == *it2) {
                 // Remove color from extracted text and return it
                 it = extracted.erase(it);
                 wasErased = true;
                 // Store color
                 color = *it2;
                 break;
             }
         }

         // Check if reached end after erasing an element
         if (it == extracted.end()) {
             break;
         }

         // Loop through modifiers and remove the mod
         for (std::vector<std::string>::iterator it2 = mods.begin();
              it2 != mods.end(); ++it2) {
             if (proposed_color == *it2) {
                 // Remove mod from extracted text
                 it = extracted.erase(it);
                 wasErased = true;
                 break;
             }
         }

         if (!color.empty()) {
            if (color == "sienna")
               return "Burnt Sienna";
            if (color == "blue") 
               return "Sky Blue"; 
            if (color == "green")
               return "Forest Green";
            color[0] = toupper(color[0]);
            return color;
         }

         if (it != extracted.end() && !wasErased)
          ++it;
     }
     return "Default";  // Unpainted
 }


// Extracts item certifications from extracted text
 std::string ItemClassifier::ExtractCertification(
     std::vector<std::string>& extracted) {

	 std::string extractedCert;

     // All available certifications
     std::vector<std::string> certs = {
         "striker",        "scorer",    "tactician",   "sweeper", "victor",
         "aviator",       "playmaker", "goalkeeper", "sniper", "paragon",
         "guardian", "acrobat",   "juggler", "show-off", "turtle"};

	 // All certification modifiers
     std::vector<std::string> mods = {
		 "certified",  "capable",   "skillfull",
         "veteran",    "fantastic", "incredible",
         "ridiculous", "daring",    "determined"};

     // Search extracted text for modifiers or certification
     std::vector<std::string>::iterator it =
         extracted.begin();
     while(it != extracted.end()) {
         std::string proposed_cert = *it;

         // Convert to lower case
         std::transform(proposed_cert.begin(), proposed_cert.end(),
                        proposed_cert.begin(), ::tolower);

         // Loop through available certs and search for match
         for (std::vector<std::string>::iterator it2 = certs.begin();
              it2 != certs.end(); ++it2) {
             if (proposed_cert == *it2) {
                 // Remove cert from extracted text and store it
                 it = extracted.erase(it);

                 // Capitalize first letter and return result
                 extractedCert = *it2;
                 extractedCert[0] = toupper(extractedCert[0]);
                 break;
             }
         }

         // Check if reached end after erasing an element
         if (it == extracted.end()) {
            break;
         }

		     // Loop through modifiers and remove the mod
         for (std::vector<std::string>::iterator it2 = mods.begin();
              it2 != mods.end(); ++it2) {
             if (proposed_cert == *it2) {
                 // Remove cert from extracted text and store it
                 it = extracted.erase(it);
                 break;
             }
         }

         if (it != extracted.end()) 
          ++it;
     }

     return extractedCert;
 }


 // Draws the base image with rendered text-detections
 void ItemClassifier::RenderTextDetections() const {
	 if (!image_.empty()) {
         // Render detections.
             std::vector<cv::RotatedRect> boxes = boxes_;
             cv::Point2f ratio((float)image_.cols / WIDTH,
                           (float)image_.rows / HEIGHT);

         for (size_t i = 0; i < indices_.size(); ++i) {
             cv::RotatedRect& box = boxes[indices_[i]];
             cv::Point2f vertices[4];
             box.points(vertices);

             for (int j = 0; j < 4; ++j) {
                 vertices[j].x *= ratio.x;
                 vertices[j].y *= ratio.y;
             }

             for (int j = 0; j < 4; ++j)
                 line(image_, vertices[j], vertices[(j + 1) % 4],
                      cv::Scalar(0, 255, 0), 1);
         }

		 // Show image
         cv::imshow("Image with Rendered Detections", image_);

		 // Wait for key press to close image
         cv::waitKey(0);
	 }
 }

 // Sanitizes words for better matching
 void ItemClassifier::Sanitize(std::string& word_or_item) {

	 // Define strange symbols to be removed
     std::vector<char> toRemove = {'[', ']', '{', '}', '!', '/', '\\','.',':',';','_'};
     
	 // Sometimes item names look like this "Item - Characteristic"
	 // In that case, be aware of the extra space after the hyphen
	 // Because this is 2 words, not 3
	 bool wasHyphen = false;

	 // Loop through word or item
     std::string::iterator it = word_or_item.begin();
     while (it != word_or_item.end()) {
         
		 // If the previous character was a hyphen, check for a space to remove
         if (wasHyphen) {

            if (*it == ' ') {
			        it = word_or_item.erase(it);
			      }

         wasHyphen = false;
         continue;
		     }

		 // Check if current char is a hyphen
         if (*it == '-') {
            wasHyphen = true;
            it = word_or_item.erase(it);
            continue;
		     }

		 // Remove any strange characters
         for (std::vector<char>::iterator it2 = toRemove.begin();
            it2 != toRemove.end(); ++it2) {

			     if (*it == *it2) {
                     it = word_or_item.erase(it);
                     break;
			     }
		     }

         if (it != word_or_item.end())
		       ++it;
	   }

	 // Convert word to lowercase
     std::transform(word_or_item.begin(), word_or_item.end(),
                word_or_item.begin(), ::tolower);
 }

 // Counts the number of words in a sanitized string
 int ItemClassifier::CountNumberOfWords(const std::string& sanitized_string) {
     if (sanitized_string.empty()) {
         return 0;
	 }

	 int count = 1;
	  
	 // Since this method assumes a sanitized string, the number of words
	 // is #spaces + 1

	 for (std::string::const_iterator it = sanitized_string.begin();
              it != sanitized_string.end(); ++it) {
		
		 // Count spaces
         if (*it == ' ') count++;
	 }
	 
	 return count;
 }

 // Splits a string on space to create a vector of words
 std::vector<std::string> ItemClassifier::SplitStringOnSpace(
	 const std::string& sanitized_string) {

	 std::vector<std::string> split;

	 if (!sanitized_string.empty()) {
        
		 std::stringstream ss;

		 // Since this method assumes a sanitized string,
		 // Split on each space
        for (const char& letter : sanitized_string) {
            
			    if (letter == ' ') {
            split.push_back(ss.str());
            ss.str(std::string());

          } else {
            ss << letter;  
			    }
		    }

        // Push the last word if needed
        if (!ss.str().empty()) 
          split.push_back(ss.str());
     }
     return split;
 }