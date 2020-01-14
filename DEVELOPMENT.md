# This is the development log. There will be a significant contrast between the first 4 weeks and subsequent dates, the latter having more specific dates as I learn how to keep a devlog.
# Week 1:
1. Log/Implemented:
  1. Installed openCV and figured out most of the methodology behind my approach
  1. Text-Detection using EAST
1. Conflicts:
  1. Program won't compile when opencv2/dnn header is included. I've tried posting on the OpenCV forums, googling for two hours, piazza, and office hours. Will see if moderator can help tomorrow
  1. Visual Studio won't show any non-cpp files in solution
1. What's Next:
  1. Figure out how to compile code
  1. Run Text-Recognition using Tesseract to extract text from bounded regions
  1. Create a JSON Database file using Python text-scraped from rl.insider.gg
1. References
  1. Where the implementation of the text-detection came from: https://github.com/spmallick/learnopencv/blob/master/TextDetectionEAST/textDetection.cpp
  1. Understanding of how the preprocessing works in blobFromImage: https://www.pyimagesearch.com/2017/11/06/deep-learning-opencvs-blobfromimage-works/

# Week 2/3:
1. Log/Implemented:
   1. Created an InventoryItem object (can be created given an item name and querying the database)
   1. Created an ItemDatabase that uses the Json file to fetch information about objects and their prices
1. Conflicts:
   1. FINALLY figured out how to properly link OpenCV (Wednesday)
1. References / Notes
   1. Used vcpkg to install jsoncpp and opencv4[dnn]. This did not properly fix my opencv issue
   1. Here are the resources that DID fix the openCV issue:
      1. Install OpenCV and extra contrib files and make with CMake: https://www.youtube.com/watch?v=x5EWlNQ6z5w&t=317s (Build and Install OpenCV 4 on Windows 10 | Adam Hacks)
      1. Link OpenCV to path variable and bin/libraries/includes with Visual Studio: https://www.youtube.com/watch?v=p-6rG6Zgu4U&t=332s (Create an OpenCV 4 Project in Visual Studio [C++] | Adam Hacks)

# Week 4:
1. Log/Implemented:
   1. Successfully extract SOME text using tesseract
   1. Successfully linked catch.hpp and tested all files
   1. Created ItemClassifier API to modularize text detection and recognition, in addition to added functionality
   1. Created Inventory API that can hold InventoryItems and prints different lists
   1. Created an in-depth README.md
1. Conflicts:
   1. Tesseract has difficulty extracting text from some images. I suspect this is because the color of the text almost matches its background and subtracting the mean of the image is insufficient. The accuracy of the text extraction may therefore vary between objects. 

# January 2020
1. Log:
  1. Changed PrettyPrint, PrintBuyingList, and PrintSellingList general output; now also return strings for printing to a file or anything