#include "utils.hpp"

#include <iostream>
#include <filesystem>
#include <string>
#include <cstring>
#include "raylib.h"

namespace utils {

// WARNING: this could break
void ParseArgs(int argc, char* argv[], Config& config) {
    for (int i = 1; i < argc; ++i) {
        if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
            std::cout << "Usage:\n";
            std::cout << "photoViewer [options] [path/value]\n\n";
            std::cout << "Options:\n";
            std::cout << "[# NOTE: the paths must end with '/' (eg: \"foo/bar/\")]\n";
            std::cout << "-i <path>     Path to jpg/png images directory\n";
            std::cout << "-r <path>     Path to raw images directory\n";
            std::cout << "-t <path>     Path to trash directory\n";
            std::cout << "-e <value>    Raw file extension (eg: \".ARW\")\n";
            std::exit(0);
        } else if (i + 1 < argc && strcmp(argv[i + 1], "") != 0) {
            // we need values following these options
            if (strcmp(argv[i], "-i") == 0) {
                // image path
                config.imageDir = argv[++i];
                continue;
            } else if (strcmp(argv[i], "-r") == 0) {
                // raw image path
                config.rawImageDir = argv[++i];
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
    const std::string ext = GetFileExtension(filePath);
    if (std::filesystem::is_directory(filePath)) {
        return false;
    } else if (ext != ".png"  &&
        ext != ".PNG"  &&
        ext != ".jpg"  &&
        ext != ".JPG"  &&
        ext != ".jpeg" &&
        ext != ".JPEG"
    ) {
        return true;
    }

    return false;
}

}
