#pragma once

#include <cstdint>
#include <cstring>
#include <string>


class Config {
public:
    explicit Config(const char* path = "./",
        const char* rawExt = ".ARW",
        const uint64_t wWidth = 1280,
        const uint64_t wHeight = 960);

    // the raw image directory is set to be the same as image directory
    // and the trash folder is created in the image directory
    void SetImageDirs(const char* path);
    void SetImageDirs(const char* imgDir, const char* rawImgDir, const char* trDir);

public:
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
    ~ImageDetails();

    // TODO: implement
    ImageDetails(const ImageDetails&);
    ImageDetails& operator= (const ImageDetails&);
    ImageDetails(ImageDetails&&);
    ImageDetails& operator= (ImageDetails&&);

public:
    std::string filepath;
    std::string filename; // filename with extension
    std::string filenameNoExt; // filename without extension
    std::string extension; // lower-case
    unsigned char* data;
    uint64_t dataSize;
};
