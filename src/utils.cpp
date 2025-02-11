#include "utils.hpp"

#include <iostream>
#include <filesystem>
#include <string>
#include <cstring>
#include "raylib.h"
#include "logger.hpp"

namespace utils {

// WARNING: this could break
// TODO: fix for passing "-i -r" or equivalent
void ParseArgs(int argc, char* argv[], Config& config) {
    for (int i = 1; i < argc; ++i) {
        // "--help" or "-h" arg
        if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
            std::cout << "Usage:\n";
            std::cout << "photoViewer [options] <path/value>\n\n";
            std::cout << "Options:\n";
            std::cout << "-i <path>     Path to jpg/png image/directory\n";
            std::cout << "-r <path>     Path to raw image directory\n";
            std::cout << "-t <path>     Path to trash directory\n"\
                         "              (the deleted files will be moved here)\n";
            std::cout << "-e <value>    Raw file extension (eg: \".ARW\")\n";
            std::exit(0);
        } else if (i + 1 < argc && strcmp(argv[i + 1], "") != 0) {
            // we need values following these options
            if (strcmp(argv[i], "-i") == 0) {
                // image path
                config.imagePath = argv[++i];
                continue;
            } else if (strcmp(argv[i], "-r") == 0) {
                // raw image path
                config.rawImagePath = argv[++i];
                continue;
            } else if (strcmp(argv[i], "-t") == 0) {
                // trash path
                config.trashDir = argv[++i];
                continue;
            } else if (strcmp(argv[i], "-e") == 0) {
                // raw image extension
                config.rawImageExt = argv[++i];
                continue;
            }
        } else {
            std::cerr << "Invalid arguments provided\n";
            std::cerr << "Run \"photoViwer --help\"\n";
            std::exit(-1);
        }
    }
}

bool IsValidImage(const char* filePath) {
    if (!std::filesystem::exists(filePath)) {
        return false;
    } else if (std::filesystem::is_directory(filePath)) {
        return false;
    }

    const char* ext = GetFileExtension(filePath);
    if (!ext) {
        return false;
    } else if (strcmp(ext, ".png") == 0  ||
        strcmp(ext, ".PNG") == 0  ||
        strcmp(ext, ".jpg") == 0  ||
        strcmp(ext, ".JPG") == 0  ||
        strcmp(ext, ".jpeg") == 0 ||
        strcmp(ext, ".JPEG") == 0
    ) {
        return true;
    }

    return false;
}

void PrintEXIFData(const tinyexif::EXIFInfo& info) {
    logger::info("Exif data:");
    logger::info("    Camera       : %s (%s)", 
        info.Make.c_str(), info.Model.c_str());
    logger::info("    Date-time    : %s", info.DateTime.c_str());
    if (info.ExposureTime < 1.0) {
        logger::info("    Shutter speed: 1/%ds", 
            static_cast<int>(1.0f / info.ExposureTime));
    } else {
        logger::info("    Shutter speed: %.2fs", info.ExposureTime);
    }
    logger::info("    Aperture     : f/%.1f", info.FNumber);
    logger::info("    ISO          : %hu", info.ISOSpeedRatings);
    logger::info("    Focal length : %dmm", static_cast<int>(info.FocalLength));
    logger::info("    Orientation  : %hu", info.Orientation);
}

}
