#pragma once

#include <vector>
#include <cstdint>
#include "raylib.h"
#include "types.hpp"

class ImageViewport {
public:
    ImageViewport(const char* path,
        const uint64_t width,
        const uint64_t height,
        const std::string& rawFilePath);

    ImageViewport(const ImageViewport&) = delete;
    ImageViewport(ImageViewport&&) = delete;
    ImageViewport& operator=(ImageViewport&) = delete;
    ImageViewport& operator=(ImageViewport&&) = delete;

    void Display();
    void CleanupImages();
    void ProcessKeybindings(const uint64_t width, const uint64_t height);
    void Resize(const uint64_t width, const uint64_t height);
    void LoadFiles(const FilePathList& files);

private:
    inline ImageDetails GetCurrentImage() const { 
        return _images[_currentImageIdx]; 
    }
    void CalcDstRectangle(const uint64_t width, const uint64_t height);
    void DeleteImage(const uint64_t width, const uint64_t height);
    void ResetCamera(const uint64_t width,const uint64_t height);

private:
    std::string _rawFilePath;
    int64_t _currentImageIdx;
    Rectangle _dstRectangle; // to fit the image to the window
    Camera2D _camera;
    std::vector<ImageDetails> _images;
};
