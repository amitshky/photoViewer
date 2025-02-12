#pragma once

#include <vector>
#include <cstdint>
#include "raylib.h"
#include "types.hpp"


struct ImageViewportInfo {
    const char* imagePath;
    const char* rawImagePath;
    const char* trashDir;
    const char* rawImageExt;

    uint64_t windowWidth;
    uint64_t windowHeight;
};

class ImageViewport {
public:
    explicit ImageViewport(const ImageViewportInfo& info);

    ImageViewport(const ImageViewport&) = delete;
    ImageViewport(ImageViewport&&) = delete;
    ImageViewport& operator=(ImageViewport&) = delete;
    ImageViewport& operator=(ImageViewport&&) = delete;

    void Init();
    void Cleanup();
    void Draw();
    void Resize(const uint64_t width, const uint64_t height);

    /**
      * Checks if `path` is a file or directory to load image(s)
      * @params `path` - file or directory path
      */
    void LoadImages(const char* path);

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
    void Reset(); // reset zoom and camera position
    void NextImage();
    void PrevImage();
    void FirstImage();
    void LastImage();
    void DeleteImage(); // delete the image and raw image (if found)
    void MoveCameraUsingMouse();

    [[nodiscard]] inline std::optional<ImageDetails> GetCurrentImageInfo() const {
        if (_images.empty())
            return std::nullopt;

        return _images[_currentImageIdx];
    }

    inline void UpdateImagePath(const char* path) { _info.imagePath = path; }
    inline void UpdateRawImagePath(const char* path) { _info.rawImagePath = path; }
    inline void UpdateTrashDir(const char* path) { _info.trashDir = path; }
    inline void UpdateRawImageExt(const char* ext) { _info.rawImageExt = ext; }


private:
    /**
     * resizes `_dstRectangle` based on the window's aspect ratio
     * this function should be called when the 
     * window gets resized or when the image is rotated
     */
    void CalcDstRectangle();

    /**
     * loads image in the current index in `_images`
     */
    void LoadCurrentImage();

    inline const ImageDetails& GetCurrentImage() const { 
        return _images[_currentImageIdx];
    }

    inline ImageDetails& GetCurrentImage() { 
        return _images[_currentImageIdx];
    }


private:
    constexpr static float _zoomVal = 0.2f;
    constexpr static int32_t _rotationVal = 90;

    ImageViewportInfo _info;
    int64_t _currentImageIdx;
    Rectangle _dstRectangle; // to fit the image to the window
    Camera2D _camera;
    std::vector<ImageDetails> _images;
    ImageRotation _imageRotation;
    ImageRotation _originalRotation;

    Texture2D _texture{};
    Rectangle _srcRectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    float _aspectRatio = 0.0f;
};
