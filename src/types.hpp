#pragma once

#include <cstdint>
#include <string>
#include "raylib.h"


struct Config {
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
    std::string directory;
    std::string filename; // filename with extension
    std::string filenameNoExt; // filename without extension
    Texture2D texture;
    float aspectRatio;
    Rectangle srcRectangle;
};
