#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <algorithm>

int main() {
  std::string image_path = cv::samples::findFile("../assets/Grid_Concept_Art.png");
  cv::Mat img = imread(image_path, cv::IMREAD_COLOR);
  if (img.empty()) {
    std::cout << "Could not read the image: " << image_path << std::endl;
    return 1;
  }

  for (int y = 0; y < img.rows; y++) {
    for (int x = 0; x < img.cols; x++) {
      cv::Vec3b& color = img.at<cv::Vec3b>(y, x); // BGR
      color[0] = std::min(255, color[0] + 50); // Blue channel 
    }
  }

  imshow("Display window", img);
  int k = cv::waitKey(0);  // Wait for a keystroke in the window
  if (k == 's') {
    imwrite("export.png", img);
  }
  return 0;
}
