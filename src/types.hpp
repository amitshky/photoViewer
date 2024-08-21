#pragma once

#include <cstdint>
#include <string>
#include "raylib.h"


struct Config {
public:
    Config(const char* path,
        const char* rawExt = ".ARW",
        const uint64_t wWidth = 1280,
        const uint64_t wHeight = 960);

    // the raw image directory is set to be the same as image directory
    // and the trash folder is created in the image directory
    inline void SetImageDirs(const char* path) {
        imageDir    = path;
        rawImageDir = imageDir;
        trashDir    = imageDir + "trash/";
    }

public:
    // WARNING: include "/" at the end of the directory name (eg: "test/pic/")
    std::string imageDir; // jpg/png directory path
    std::string rawImageDir; // raw image directory path
    std::string trashDir; // path to move images when deleted
    std::string rawImageExt; // extension of the raw image (eg: ".ARW")

    uint64_t windowWidth;
    uint64_t windowHeight;
};

struct ImageDetails {
public:
    ImageDetails(const char* path);

public:
    std::string filepath;
    std::string directory; // TODO: this may not be needed, remove it
    std::string filename; // filename with extension
    std::string filenameNoExt; // filename without extension
    Texture2D texture;
    float aspectRatio;
    Rectangle srcRectangle;
};
