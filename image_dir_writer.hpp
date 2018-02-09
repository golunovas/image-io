#ifndef IMAGE_DIR_WRITER_HPP_
#define IMAGE_DIR_WRITER_HPP_

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

class ImageDirWriter {
private:
	boost::lockfree::queue<ImageData*, boost::lockfree::fixed_sized<true>> imageQueue_;
	bool isTerminated_;
	std::thread imageWriterThread_;
	void writeImages();
public:
	ImageDirWriter(const ImageDirReader&) = delete;
	ImageDirWriter& operator=(const ImageDirReader&) = delete;

	ImageDirWriter(size_t queueSize = 256);
	~ImageDirWriter();
	void writeImage(const ImageData& imageData);
};

inline ImageDirWriter::ImageDirWriter(size_t queueSize) : imageQueue_(queueSize), isTerminated_(false) {
	imageWriterThread_ = std::move(std::thread(&ImageDirWriter::writeImages, this));
}

inline ImageDirWriter::~ImageDirWriter() {
	isTerminated_ = true;
	imageWriterThread_.join();
}

inline void ImageDirWriter::writeImage(const ImageData& imageData) {
	ImageData* tmpImageData = new ImageData(imageData);
	while (!isTerminated_ && !imageQueue_.push(tmpImageData)) { }
}

inline void ImageDirWriter::writeImages() {
	while (true) {
		ImageData *imageData = nullptr;
		if (imageQueue_.pop(imageData) && imageData != nullptr) {
			std::string imagePathStr = imageData->imagePath;
			cv::Mat img = imageData->img;
			delete imageData;
			try {
				fs::path imagePath(imagePathStr);
				fs::create_directories(imagePath.parent_path());
				cv::imwrite(imagePathStr.c_str(), img);
			} catch (...) {
				// NOTE: Would be good to know what happened.
			}
		}
		if (isTerminated_ && imageQueue_.empty()) {
			break;
		}
	}
}

} // namespace image_io

#endif // IMAGE_DIR_WRITER_HPP_