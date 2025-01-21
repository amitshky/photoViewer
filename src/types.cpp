#include "types.hpp"

#include <filesystem>
#include "raylib.h"

Config::Config(const char* path,
    const char* rawExt,
    const uint64_t wWidth,
    const uint64_t wHeight)
    : rawImageExt{ rawExt },
      windowWidth{ wWidth },
      windowHeight{ wHeight } {
    InitImageDirs(path);
}

void Config::InitImageDirs(const char* path) {
    // if no path is specified, set it as current path
    std::string imgDir{ path };
    if (imgDir.empty()) {
        imgDir = std::filesystem::current_path().string();
    }

    if (imgDir[imgDir.length() - 1] == '/' ) {
        imagePath = imgDir;
    } else {
        imagePath = imgDir + '/';
    }

    rawImagePath = imagePath;
    trashDir = imagePath + "trash/";
}


ImageDetails::ImageDetails(const char* path)
    : filepath{ path },
      filename{ "" },
      filenameNoExt{ "" },
      extension{ "" } {
    filename = GetFileName(path);
    extension = GetFileExtension(filename.c_str());
    for (const auto& ch : filename) {
        if (ch == '.')
            break;

        filenameNoExt += ch;
    }
}

