#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <string>

#include "ofApp.h"
#include "ofMain.h"

constexpr auto MODEL =
    "frozen_east_text_detection.pb";  // The location of the model used to train
                                      // text detection
constexpr int HEIGHT = 320;  // Resizes preprocessed image to this height
constexpr int WIDTH = 320;   // Resizes preprocessed image to this width
constexpr auto PATH_TO_TEST_IMAGES =
    "..\\Test Images for RL\\Isolated";  // Folder containing isolated test
                                         // images
constexpr auto TEST_IMAGE_1 = "BareOctaneMG88.png";
constexpr float IMAGE_SCALE = 1.0; // Preprocessing image scale factor
constexpr float NETWORK_SIZE = 320; // Preprocessing model network size - experiment to improve accuracy
constexpr float CONFIDENCE_THRESHOLD = .50; // How confident we want to be that our text box properly encloses the text
constexpr float NON_MAX_SUPPRESSION_THRESHOLD = .40; // 

int main() {
    /* Note:
            Tesseract is a popular Text Recognition model that maps an image of
       text to the actual content text. Tesseract requires a bounded region
       within which to search for text. EAST Text Detection will detect and
       bound the region containing text. Tesseract can then search within that
       region for text.
    */

    /* References:
    https://www.learnopencv.com/deep-learning-based-text-detection-using-opencv-c-python/
    or https://github.com/spmallick/learnopencv/blob/master/TextDetectionEAST/textDetection.cpp
    */

    /* PROTOTYPE - most basic proof of concept */

    /* ======================================= TEXT-DETECTION =======================================*/

    // Load the model - EAST Text Detection
    cv::dnn::Net net = cv::dnn::readNet(MODEL);

    // Load in a test image
    const std::string path_to_images = PATH_TO_TEST_IMAGES;
    cv::Mat image = cv::imread(path_to_images + TEST_IMAGE_1);

    /* The next function preprocesses an image.
        The link below explains exactly how this works, but essentially,
        preprocessing is a multi-step process that "helps combat illumination
    changes"
    https://www.pyimagesearch.com/2017/11/06/deep-learning-opencvs-blobfromimage-works/
    */

    // Preprocess the image to ready it for reading
    cv::Mat output_image;
    cv::dnn::blobFromImage(image, output_image, IMAGE_SCALE,
                           cv::Size(WIDTH, HEIGHT),
                           cv::Scalar(123.68, 116.78, 103.94), true, false);

    // Display the images
    cv::imshow("Unprocessed Image",image);
    cv::imshow("Preprocessed Image", output_image);

    // Specify the output layers for the network
    std::vector<cv::String> outputLayers(2);
    outputLayers[0] = "feature_fusion/Conv_7/Sigmoid"; // Geometry of the text box
    outputLayers[1] = "feature_fusion/concat_3"; // Confidence of the detected box

    // Pass the input image through the network and obtain geometry and confidence scores
    std::vector<cv::Mat> output;
    net.setInput(output_image);
    net.forward(output, outputLayers);
    cv::Mat scores = output[0];
    cv::Mat geometry = output[1];

    // Decode the positions and orientations of the text boxes 
    std::vector<cv::RotatedRect> boxes;
    std::vector<float> confidences;
    decode(scores, geometry, CONFIDENCE_THRESHOLD, boxes, confidences);

    // Filter out the best candidates for the correct text box using Non-Maximum Suppression
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, CONFIDENCE_THRESHOLD,
                 NON_MAX_SUPPRESSION_THRESHOLD,
                 indices);

    // Render text boxes.
    cv::Point2f ratio((float)image.cols / WIDTH,
                      (float)image.rows / HEIGHT);
    for (size_t i = 0; i < indices.size(); ++i) {
        cv::RotatedRect& box = boxes[indices[i]];

        cv::Point2f vertices[4];
        box.points(vertices);
        for (int j = 0; j < 4; ++j) {
            vertices[j].x *= ratio.x;
            vertices[j].y *= ratio.y;
        }
        for (int j = 0; j < 4; ++j)
            line(image, vertices[j], vertices[(j + 1) % 4],
                 cv::Scalar(0, 255, 0),
                 2, cv::LINE_AA);
    }

    // Print image with detections
    cv::imshow("Text-Detection", image);
}


// Decode the positions and orientations of the text boxes
// Ref: https://github.com/spmallick/learnopencv/blob/master/TextDetectionEAST/textDetection.cpp
void decode(const cv::Mat& scores, const cv::Mat& geometry, float scoreThresh,
            std::vector<cv::RotatedRect>& detections,
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
            // Multiple by 4 because feature maps are 4 times less than input
            // image.
            float offsetX = x * 4.0f, offsetY = y * 4.0f;
            float angle = anglesData[x];
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);
            float h = x0_data[x] + x2_data[x];
            float w = x1_data[x] + x3_data[x];

            cv::Point2f offset(offsetX + cosA * x1_data[x] + sinA * x2_data[x],
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
