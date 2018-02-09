#include <chrono>
#include <iostream>

#include "helpers.hpp"
#include "image_dir_reader.hpp"

int main(int argc, char** argv) {
	if (argc < 3) {
		std::cout << "usage: " << argv[0] << " <image dir> <job delay ms>" << std::endl;
		return 0;
	}
	std::string imageDir = argv[1];
	int jobDelay = std::atoi(argv[2]);
	Timer timer;

	timer.start();
	{
		image_io::ImageDirReader imageDirReader(imageDir, 0);
		const auto& imagePaths = imageDirReader.getImagePaths();
		for (const auto& imagePath : imagePaths) {
			cv::Mat img = cv::imread(imagePath);
			std::this_thread::sleep_for(std::chrono::milliseconds(jobDelay));
		}
	}	
	std::cout << "Baseline: " << timer.stop() << std::endl;

	timer.start();
	{
		image_io::ImageDirReader imageDirReader(imageDir, 1);
		while (!imageDirReader.isEmpty()) {
			auto imageData = imageDirReader.getImage();
			std::this_thread::sleep_for(std::chrono::milliseconds(jobDelay));
		}
	}	
	std::cout << "Queue size 1: " << timer.stop() << std::endl;

	timer.start();
	{
		image_io::ImageDirReader imageDirReader(imageDir, 256);
		while (!imageDirReader.isEmpty()) {
			auto imageData = imageDirReader.getImage();
			std::this_thread::sleep_for(std::chrono::milliseconds(jobDelay));
		}
	}
	std::cout << "Queue size 256: " << timer.stop() << std::endl;

	return 0;
}