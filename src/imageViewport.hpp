#pragma once

#include <vector>
#include <cstdint>
#include "raylib.h"
#include "types.hpp"

class ImageViewport {
public:
    ImageViewport(const char* path, const uint64_t width, const uint64_t height);

    ImageViewport(const ImageViewport&) = delete;
    ImageViewport(ImageViewport&&) = delete;
    ImageViewport& operator=(ImageViewport&) = delete;
    ImageViewport& operator=(ImageViewport&&) = delete;

    void Display();
    void CleanupImages();
    void ProcessKeybindings(uint64_t& width,
        uint64_t& height,
        float& winAspectRatio,
        const std::string& rawFilePath);
    void Resize(uint64_t width, uint64_t height, float winAspectRatio);
    void LoadFiles(const FilePathList& files);

private:
    std::vector<ImageDetails> _images;
    int64_t _currentImageIdx;
    Camera2D _camera;
    Rectangle _dstRectangle; // used to display the texture in the viewport
};
