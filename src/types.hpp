#pragma once

#include <cstdint>
#include <string>
#include "raylib.h"


struct Config {
    std::string imagePath; // jpg/png directory path
    std::string rawImagePath; // raw image directory path
    std::string trashPath; // path to move images when deleted

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
