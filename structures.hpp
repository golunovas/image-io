#ifndef STRUCTURES_HPP_
#define STRUCTURES_HPP_

namespace image_io {

struct ImageData {
	std::string imagePath;
	cv::Mat img;
	ImageData() {}
	ImageData(const std::string& imagePath, cv::Mat img) : imagePath(imagePath), img(img) { }
	ImageData(const ImageData& other) { imagePath = other.imagePath; img = other.img; }
	ImageData& operator=(const ImageData& other) { imagePath = other.imagePath; img = other.img; return *this; }
};

}

#endif // STRUCTURES_HPP_