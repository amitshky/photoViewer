#pragma once

#include <string>
#include "raylib.h"


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
