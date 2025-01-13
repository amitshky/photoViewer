#pragma once

#include <cstdint>
#include <cstring>
#include <string>


class Config {
public:
    explicit Config(const char* path = "",
        const char* rawExt = ".ARW", // default is sony's raw file extension
        const uint64_t wWidth = 1280,
        const uint64_t wHeight = 960);

public:
    std::string imagePath; // jpg/png directory path or file path
    std::string rawImagePath; // raw image directory path
    std::string trashDir; // path to move images when deleted
    std::string rawImageExt; // extension of the raw image (eg: ".ARW")

    uint64_t windowWidth;
    uint64_t windowHeight;

private:
    /**
     * Initializes the image directories (image path, raw image path, and trash
     * directory path. The raw image directory is set same as the image directory
     * and the trash directory is created in the image directory.
     *
     * @param `path` - path of the image or image directory
     */
    void InitImageDirs(const char* path);
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
