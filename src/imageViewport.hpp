#pragma once

#include <vector>
#include <cstdint>
#include "raylib.h"
#include "tinyexif/exif.h"
#include "types.hpp"


class ImageViewport {
public:
    explicit ImageViewport(const ImageViewportInfo& info);

    ImageViewport(const ImageViewport&) = delete;
    ImageViewport(ImageViewport&&) = delete;
    ImageViewport& operator=(ImageViewport&) = delete;
    ImageViewport& operator=(ImageViewport&&) = delete;

    void Draw();
    void Cleanup();
    void Resize(const uint64_t width, const uint64_t height);

    /**
      * Loads images using the path of the image
      * @param `filePath` - file path
      */
    void LoadFile(const char* filePath);

    /**
      * Loads images from path list (for dropped files)
      * @param `files` - list of paths
      */
    void LoadFilesFromList(const FilePathList& files);

    /**
      * Loads images using directory path
      * @param `path` - path of the directory the images are in
      */
    void LoadFilesFromDir(const char* path);

    void ZoomIn();
    void ZoomOut();
    void ResetZoom(); // reset the zoom of the image
    void RotateCW(); // rotate image clockwise
    void RotateCCW(); // rotate image counter clockwise
    void ResetImage(); // reset zoom and camera position
    void NextImage();
    void PrevImage();
    void DeleteImage(); // delete the image and raw image (if found)
    void MoveCameraUsingMouse();

    [[nodiscard]] inline ImageDetails GetCurrentImageInfo() const {
        return _images[_currentImageIdx];
    }

    inline void UpdateImagePath(const char* path) { _info.imagePath = path; }
    inline void UpdateRawImagePath(const char* path) { _info.rawImagePath = path; }
    inline void UpdateTrashDir(const char* path) { _info.trashDir = path; }


private:
    inline const ImageDetails& GetCurrentImage() const { 
        return _images[_currentImageIdx];
    }

    inline ImageDetails& GetCurrentImage() { 
        return _images[_currentImageIdx];
    }

    void CalcDstRectangle();
    void ResetCamera();
    void LoadCurrentImage();

private:
    constexpr static float _zoomVal = 0.2f;
    constexpr static int32_t _rotationVal = 90;
    ImageViewportInfo _info;
    int64_t _currentImageIdx;
    Rectangle _dstRectangle; // to fit the image to the window
    Camera2D _camera;
    std::vector<ImageDetails> _images;
    int32_t _imageRotation;

    Texture2D _texture{};
    Rectangle _srcRectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    float _aspectRatio = 0.0f;
};
