#include <chrono>
#include <iostream>

#include <boost/algorithm/string/replace.hpp>

#include "helpers.hpp"
#include "image_dir_reader.hpp"
#include "image_dir_writer.hpp"

int main(int argc, char** argv) {
	if (argc < 3) {
		std::cout << "usage: " << argv[0] << " <image dir> <output dir>" << std::endl;
		return 0;
	}
	std::string imageDir = argv[1];
	std::string outputDir = argv[2];
	image_io::ImageDirReader imageDirReader(imageDir, 256);
	image_io::ImageDirWriter imageDirWriter(256);
	while (!imageDirReader.isEmpty()) {
		auto imageData = imageDirReader.getImage();
		boost::replace_all(imageData.imagePath, imageDir, outputDir);
		imageDirWriter.writeImage(imageData);
	}
	return 0;
}