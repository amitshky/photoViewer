#pragma once

#include <vector>
#include <cstdint>
#include "raylib.h"
#include "types.hpp"

class ImageViewport {
public:
    ImageViewport(const char* path,
        const uint64_t width, // window width
        const uint64_t height, // window height
        const std::string& rawFilePath);

    ImageViewport(const ImageViewport&) = delete;
    ImageViewport(ImageViewport&&) = delete;
    ImageViewport& operator=(ImageViewport&) = delete;
    ImageViewport& operator=(ImageViewport&&) = delete;

    void Display();
    void CleanupImages();
    void ProcessKeybindings();
    void Resize(const uint64_t width, const uint64_t height);
    void LoadFiles(const FilePathList& files);


private:
    inline ImageDetails GetCurrentImage() const { 
        return _images[_currentImageIdx]; 
    }
    void CalcDstRectangle();
    void DeleteImage();
    void ResetCamera();

private:
    std::string _rawFilePath;
    uint64_t _windowWidth;
    uint64_t _windowHeight;
    int64_t _currentImageIdx;
    Rectangle _dstRectangle; // to fit the image to the window
    Camera2D _camera;
    std::vector<ImageDetails> _images;
};
