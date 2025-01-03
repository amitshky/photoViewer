#pragma once

#include <vector>
#include <cstdint>
#include "raylib.h"
#include "tinyexif/exif.h"
#include "types.hpp"

class ImageViewport {
public:
    explicit ImageViewport(const Config& config);

    ImageViewport(const ImageViewport&) = delete;
    ImageViewport(ImageViewport&&) = delete;
    ImageViewport& operator=(ImageViewport&) = delete;
    ImageViewport& operator=(ImageViewport&&) = delete;

    void Display();
    void CleanupImages();
    void ProcessKeybindings();
    void Resize(const uint64_t width, const uint64_t height);

    /**
      * Loads images from path list (for dropped files)
      * @param `files` - list of paths
      */
    void LoadFiles(const FilePathList& files);
    /**
      * Loads images using directory path
      * @param `path` - path of the directory the images are in
      */
    void LoadFiles(const char* path);

    /**
      * Loads images using the path of the image
      * @param `filePath` - file path
      */
    void LoadFile(const char* filePath);

private:
    // TODO: make this function const
    inline ImageDetails& GetCurrentImage() { 
        return _images[_currentImageIdx]; 
    }
    void CalcDstRectangle();
    void DeleteImage();
    void ResetCamera();
    void LoadCurrentImage(const char* path);
    void UnloadCurrentImage();
    static void PrintEXIFData(const tinyexif::EXIFInfo& data);

private:
    Config _config;
    int64_t _currentImageIdx;
    Rectangle _dstRectangle; // to fit the image to the window
    Camera2D _camera;
    std::vector<ImageDetails> _images;
    int32_t _imageRotation;

    Texture2D _texture{};
    Rectangle _srcRectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    float _aspectRatio = 0.0f;
};
