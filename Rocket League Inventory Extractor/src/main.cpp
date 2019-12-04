#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <string>

#include "ofApp.h"
#include "ofMain.h"

using namespace cv;
using namespace cv::dnn;

constexpr auto MODEL =
    "C:\\Users\\Unknown_User\\Documents\\openFrameworks\\apps\\fantastic-finale-astudent82828211\\Rocket League Inventory Extractor\\frozen_east_text_detection.pb";  // The location of the model used to train
                                      // text detection
constexpr int HEIGHT = 320;  // Resizes preprocessed image to this height
constexpr int WIDTH = 320;   // Resizes preprocessed image to this width
constexpr auto PATH_TO_TEST_IMAGES =
    "C:\\Users\\Unknown_User\\Documents\\openFrameworks\\apps\\fantastic-finale-astudent82828211\\Rocket League Inventory Extractor\\Test Images for RL\\Isolated\\";  // Folder containing isolated test
                                         // images
constexpr auto TEST_IMAGE_1 = "CertJagerXVI.png";
constexpr float IMAGE_SCALE = 1.0; // Preprocessing image scale factor
constexpr float NETWORK_SIZE = 320; // Preprocessing model network size - experiment to improve accuracy
constexpr float CONFIDENCE_THRESHOLD = .50; // How confident we want to be that our text box properly encloses the text
constexpr float NON_MAX_SUPPRESSION_THRESHOLD = .45; // This will change detection accuracy and the number of text boxes made

// Decode the positions and orientations of the text boxes
// Ref:
// https://github.com/spmallick/learnopencv/blob/master/TextDetectionEAST/textDetection.cpp
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
            // Multiple by 4 because feature maps are 4 time less than input
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

int main(int argc, char** argv) {
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

   /* ======================================= text-detection =======================================*/

float confThreshold = CONFIDENCE_THRESHOLD;
float nmsThreshold = NON_MAX_SUPPRESSION_THRESHOLD;
int inpWidth = WIDTH;
int inpHeight = HEIGHT;
String model = MODEL;
CV_Assert(!model.empty());
// Load network.
Net net = readNet(model);
static const std::string kWinName =
    "EAST: An Efficient and Accurate Scene Text Detector";
namedWindow(kWinName, WINDOW_NORMAL);
// Specify the output layers for the network
std::vector<Mat> outs;
std::vector<String> outNames(2);
outNames[0] = "feature_fusion/Conv_7/Sigmoid"; // Geometry
outNames[1] = "feature_fusion/concat_3"; // Confidence
Mat frame, blob;
// Load in a test image
const std::string path_to_images = PATH_TO_TEST_IMAGES;
const std::string image1 = path_to_images + TEST_IMAGE_1;
frame = cv::imread(image1);
/* Preprocesses an image.
      The link below explains exactly how this works, but essentially,
      preprocessing is a multi-step process that "helps combat illumination changes"
  https://www.pyimagesearch.com/2017/11/06/deep-learning-opencvs-blobfromimage-works/
  */
blobFromImage(frame, blob, 1.0, Size(inpWidth, inpHeight),
            cv::mean(frame), true, false);
// Pass the input image through the network and obtain geometry and confidence scores
net.setInput(blob);
net.forward(outs, outNames);
Mat scores = outs[0];
Mat geometry = outs[1];
// Decode predicted bounding boxes.
std::vector<RotatedRect> boxes;
std::vector<float> confidences;
decode(scores, geometry, confThreshold, boxes, confidences);
// Filter out the best candidates for the correct text box using non-maximum suppression
std::vector<int> indices;
NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
// Render detections.
Point2f ratio((float)frame.cols / inpWidth, (float)frame.rows / inpHeight);
for (size_t i = 0; i < indices.size(); ++i) {
    RotatedRect& box = boxes[indices[i]];
    Point2f vertices[4];
    box.points(vertices);
    for (int j = 0; j < 4; ++j) {
        vertices[j].x *= ratio.x;
        vertices[j].y *= ratio.y;
    }
    for (int j = 0; j < 4; ++j)
        line(frame, vertices[j], vertices[(j + 1) % 4], Scalar(0, 255, 0),
            1);
}

// Detected

imshow(kWinName, frame);
cv::waitKey(0);

 return 0;
}