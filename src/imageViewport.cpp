#include "imageViewport.hpp"

#include <filesystem>
#include "logger.hpp"
#include "raylib.h"

ImageViewport::ImageViewport(const Config& config)
    : _config{ config },
      _currentImageIdx{ 0 },
      _dstRectangle{ 0.0f,  0.0f, 0.0f, 0.0f },
      _camera{},
      _images{}
    {
    FilePathList files = LoadDirectoryFiles(_config.imagePath.c_str());
    LoadFiles(files);
    ResetCamera();
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

void ImageViewport::ProcessKeybindings() {
    const float scroll = GetMouseWheelMove();
    constexpr float zoomVal = 0.2f;
    constexpr float rotationVal = 90.0f;

    // "scroll down" or "-" or "S" to zoom out
    if ((scroll < 0.0f || IsKeyDown(KEY_MINUS) || IsKeyPressed(KEY_S)) && _camera.zoom > zoomVal) {
        _camera.zoom -= zoomVal;
    }
    // "scroll up" or "+" or "W" to zoom in
    else if ((scroll > 0.0f || IsKeyDown(KEY_EQUAL) || IsKeyPressed(KEY_W)) && _camera.zoom <= 100.0f) {
        _camera.zoom += zoomVal;
    }
    // "0" to reset zoom
    else if (IsKeyPressed(KEY_ZERO)) {
        _camera.zoom = 1.0f;
    }
    // "]" or "E" to rotate clockwise
    else if (IsKeyPressed(KEY_RIGHT_BRACKET) || IsKeyPressed(KEY_E)) {
        // TODO: rotate the image not the camera
        // TODO: fit to window after rotating
        _camera.rotation += rotationVal;
        _camera.rotation = static_cast<float>(static_cast<int32_t>(_camera.rotation) % 360);
    }
    // "[" or "Q" to rotate counter clockwise
    else if (IsKeyPressed(KEY_LEFT_BRACKET) || IsKeyPressed(KEY_Q)) {
        _camera.rotation -= rotationVal;
        _camera.rotation = static_cast<float>(static_cast<int32_t>(_camera.rotation) % 360);
    }
    // "R" to reset camera
    else if (IsKeyPressed(KEY_R)) {
        ResetCamera();
    }
    // "D" or "Right arrow" to view next image
    else if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && _currentImageIdx + 1 < _images.size()) {
        ++_currentImageIdx;
        CalcDstRectangle();
    }
    // "A" or "Left arrow" to view previous image
    else if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && _currentImageIdx - 1 >= 0) {
        --_currentImageIdx;
        CalcDstRectangle();
    }
    // "Delete" or "X" to delete image as well as raw image (if exists)
    else if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_X)) {
        DeleteImage();
    }
}

void ImageViewport::Resize(const uint64_t width, const uint64_t height) {
    _config.windowWidth = width;
    _config.windowHeight = height;
    _camera.offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f };
    CalcDstRectangle();
}

void ImageViewport::LoadFiles(const FilePathList& files) {
    if (files.count <= 0)
        return;

    CleanupImages();
    if (!_images.empty()) {
        _images.clear();
        _images.reserve(files.count);
    }

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
    CalcDstRectangle();

    if (_images.empty()) {
        logger::info("No images found!");
    }
}

void ImageViewport::CalcDstRectangle() {
    if (_images.empty())
        return;

    const float w = static_cast<float>(_config.windowWidth);
    const float h = static_cast<float>(_config.windowHeight);
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

void ImageViewport::DeleteImage() {
    if (_images.empty())
        return;

    // TODO: do not hardcode
    const char* trash = "test/trash/";
    if (!std::filesystem::exists(trash)) {
        std::filesystem::create_directory(trash);
    }

    std::filesystem::path imagePath{ GetCurrentImage().filepath };
    std::filesystem::path rawImagePath{ _config.rawImagePath + GetCurrentImage().filenameNoExt + ".ARW" };

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
    CalcDstRectangle();
}

void ImageViewport::ResetCamera() {
    _camera.offset = Vector2{
        .x = static_cast<float>(_config.windowWidth) * 0.5f,
        .y = static_cast<float>(_config.windowHeight) * 0.5f 
    };
    _camera.target = Vector2{ 0.0f, 0.0f };
    _camera.rotation = 0.0f;
    _camera.zoom = 1.0f;
}
