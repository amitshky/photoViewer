#include "imageViewport.hpp"

#include <filesystem>
#include "logger.hpp"
#include "raylib.h"

ImageViewport::ImageViewport(const char* path,
    const uint64_t width,
    const uint64_t height,
    const std::string& rawFilePath)
    : _rawFilePath{rawFilePath},
      _currentImageIdx{ 0 },
      _dstRectangle{ 0.0f,  0.0f, 0.0f, 0.0f },
      _camera{},
      _images{}
    {
    FilePathList files = LoadDirectoryFiles(path);
    LoadFiles(files);

    if (_images.empty()) {
        logger::info("No images found in the current directory!");
    }

    CalcDstRectangle(width, height);
    ResetCamera(width, height);
} 

void ImageViewport::Display() {
    if (_images.empty())
        return;

    BeginMode2D(_camera);

    DrawTexturePro(GetCurrentImage().texture,
        GetCurrentImage().srcRectangle,
        _dstRectangle,
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );

    EndMode2D();
}

void ImageViewport::CleanupImages() {
    for (const auto& img : _images) {
        UnloadTexture(img.texture);
    }
}

void ImageViewport::ProcessKeybindings(const uint64_t width,
    const uint64_t height) {
    const float scroll = GetMouseWheelMove();

    if ((scroll < 0.0f || IsKeyDown(KEY_MINUS)) && _camera.zoom > 0.5f) { // "scroll down" or "-" to zoom out
        _camera.zoom -= 0.5f;
    } else if ((scroll > 0.0f || IsKeyDown(KEY_EQUAL)) && _camera.zoom <= 100.0f) { // "scroll up" or "+" to zoom in
        _camera.zoom += 0.5f;
    } else if (IsKeyPressed(KEY_ZERO)) { // "0" to reset zoom
        _camera.zoom = 1.0f;
    } else if (IsKeyPressed(KEY_RIGHT_BRACKET)) { // "]" to rotate clockwise
        // TODO: rotate the image not the camera
        // TODO: fit to window after rotating
        _camera.rotation += 90.0f;
        _camera.rotation = static_cast<float>(static_cast<int32_t>(_camera.rotation) % 360);
    } else if (IsKeyPressed(KEY_LEFT_BRACKET)) { // "[" to rotate counter clockwise
        _camera.rotation -= 90.0f;
        _camera.rotation = static_cast<float>(static_cast<int32_t>(_camera.rotation) % 360);
    } else if (IsKeyPressed(KEY_R)) { // "R" to reset camera
        ResetCamera(width, height);
    } else if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && _currentImageIdx + 1 < _images.size()) { // "D" or "Right arrow" to view next image
        ++_currentImageIdx;
        CalcDstRectangle(width, height);
    } else if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && _currentImageIdx - 1 >= 0) { // "A" or "Left arrow" to view previous image
        --_currentImageIdx;
        CalcDstRectangle(width, height);
    } else if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_X)) { // "Delete" or "X" to delete image as well as raw image (if exists)
        DeleteImage(width, height);
    }
}

void ImageViewport::Resize(const uint64_t width, const uint64_t height) {
    _camera.offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f };
    CalcDstRectangle(width, height);
}

void ImageViewport::LoadFiles(const FilePathList& files) {
    if (files.count > 0) {
        CleanupImages();
        _images.clear();
        _images.reserve(files.count);

        for (uint64_t i = 0; i < files.count; ++i) {
            const char* path = files.paths[i];

            if (std::filesystem::is_directory(path)) {
                continue;
            } else if (!std::filesystem::is_regular_file(path)) {
                continue;
            }

            // check if the file is png/jpg or not
            const std::string ext = GetFileExtension(path);
            if (ext != ".png"  &&
                ext != ".PNG"  &&
                ext != ".jpg"  &&
                ext != ".JPG"  &&
                ext != ".jpeg" &&
                ext != ".JPEG"
            ) {
                continue;
            }

            _images.emplace_back(path);
        }

        _currentImageIdx = 0;
    }
}

void ImageViewport::CalcDstRectangle(const uint64_t width, const uint64_t height) {
    if (_images.empty())
        return;

    const float h = static_cast<float>(height);
    const float w = static_cast<float>(width);
    const float winAspectRatio = w / h;

    if (GetCurrentImage().aspectRatio < winAspectRatio) {
        _dstRectangle.x      = -h * 0.5f * GetCurrentImage().aspectRatio;
        _dstRectangle.y      = -h * 0.5f;
        _dstRectangle.width  = h * GetCurrentImage().aspectRatio;
        _dstRectangle.height = h;
    } else {
        _dstRectangle.x      = -w * 0.5f;
        _dstRectangle.y      = -w * 0.5f * 1.0f / GetCurrentImage().aspectRatio;
        _dstRectangle.width  = w;
        _dstRectangle.height = w * 1.0f / GetCurrentImage().aspectRatio;
    }
}

void ImageViewport::DeleteImage(const uint64_t width, const uint64_t height) {
    if (_images.empty())
        return;

    // TODO: do not hardcode
    const char* trash = "test/trash/";
    if (!std::filesystem::exists(trash)) {
        std::filesystem::create_directory(trash);
    }

    std::filesystem::path imagePath{ GetCurrentImage().filepath };
    std::filesystem::path rawImagePath{ _rawFilePath + GetCurrentImage().filenameNoExt + ".ARW" };

    // move the files to `.trash`
    std::filesystem::rename(imagePath, trash + GetCurrentImage().filename);
    logger::log("deleting: \"%s\"", imagePath.c_str());
    if (std::filesystem::exists(rawImagePath)) {
        logger::log("deleting: \"%s\"", rawImagePath.c_str());
        std::filesystem::rename(rawImagePath, trash + GetCurrentImage().filenameNoExt + ".ARW");
    }

    // FIXME: after deleting, the next image's aspect ratio is not loading correctly
    _images.erase(_images.begin() + _currentImageIdx);
    if (_currentImageIdx - 1 >= 0) {
        --_currentImageIdx;
    } else {
        _currentImageIdx = 0;
    }
    CalcDstRectangle(width, height);
}

void ImageViewport::ResetCamera(const uint64_t width,const uint64_t height) {
    _camera.offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f };
    _camera.target = Vector2{ 0.0f, 0.0f };
    _camera.rotation = 0.0f;
    _camera.zoom = 1.0f;
}
