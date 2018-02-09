#ifndef IMAGE_DIR_READER_HPP_
#define IMAGE_DIR_READER_HPP_

#include <mutex>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <utility>

#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/algorithm/string.hpp>

#include "structures.hpp"

namespace fs = boost::filesystem;

namespace image_io {

const std::set<std::string> IMAGE_EXTENSIONS = {".png", ".jpg", ".jpeg", ".PNG", ".JPG", ".JPEG"};

class ImageDirReader {
private:
	size_t readCount_;
	boost::lockfree::queue<ImageData*, boost::lockfree::fixed_sized<true>> imageQueue_;
	bool isTerminated_;
	std::thread imageReaderThread_;
	std::vector<std::string> imagePaths_;
	void loadImages();
	void loadDirList(const std::string& imageDir);

	static bool isImage(const fs::path& filePath);
public:
	ImageDirReader() = delete;
	ImageDirReader(const ImageDirReader&) = delete;
	ImageDirReader& operator=(const ImageDirReader&) = delete;

	ImageDirReader(const std::string& imageDir, size_t queueSize = 256);
	~ImageDirReader();
	ImageData getImage();
	size_t getSize();
	bool isEmpty();
	const std::vector<std::string>& getImagePaths() const;
};

inline ImageDirReader::ImageDirReader(const std::string& imageDir, size_t queueSize) : readCount_(0), imageQueue_(queueSize), isTerminated_(false) {
	loadDirList(imageDir);
	imageReaderThread_ = std::move(std::thread(&ImageDirReader::loadImages, this));
}

inline ImageDirReader::~ImageDirReader() {
	isTerminated_ = true;
	imageReaderThread_.join();
}

inline ImageData ImageDirReader::getImage() {
	ImageData* imageData = nullptr;
	while (!imageQueue_.pop(imageData) && (readCount_ < imagePaths_.size())) { }
	readCount_++;
	if (imageData == nullptr) { return ImageData(); }
	ImageData result(*imageData);
	delete imageData;
	return result;
}

inline size_t ImageDirReader::getSize() {
	return imagePaths_.size();
}

inline bool ImageDirReader::isEmpty() {
	return (readCount_ >= imagePaths_.size());
}

inline const std::vector<std::string>& ImageDirReader::getImagePaths() const {
	return imagePaths_;
}

inline void ImageDirReader::loadImages() {
	for (auto& imagePath : imagePaths_) {
		ImageData* imageData = nullptr;
		cv::Mat img;
		try {
			img = cv::imread(imagePath);
			imageData = new ImageData(imagePath, img);
		} catch (...) { }
		while (!isTerminated_ && !imageQueue_.push(imageData)) { }
	}
}

inline bool ImageDirReader::isImage(const fs::path& filePath) {
	return (IMAGE_EXTENSIONS.find(filePath.extension().string()) != IMAGE_EXTENSIONS.end());
}

inline void ImageDirReader::loadDirList(const std::string& imageDir) {
	fs::recursive_directory_iterator endItr;
	fs::path directory(imageDir);
	for (fs::recursive_directory_iterator i(directory); i != endItr; ++i) {
		if (fs::is_regular_file(i->status()) && isImage(i->path())) {
			imagePaths_.push_back(i->path().string());
		}
	}
}

} // namespace image_io

#endif // IMAGE_DIR_READER_HPP_