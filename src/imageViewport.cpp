#include "imageViewport.hpp"

#include <filesystem>
#include "raylib.h"
#include "logger.hpp"
#include "utils.hpp"

ImageViewport::ImageViewport(const Config& config)
    : _config{ config },
      _currentImageIdx{ 0 },
      _dstRectangle{ 0.0f,  0.0f, 0.0f, 0.0f },
      _camera{},
      _images{} {
    if (std::filesystem::is_directory(_config.imageDir)) {
        LoadPath(_config.imageDir.c_str());
    } else if (std::filesystem::is_regular_file(_config.imageDir)) {
        LoadFile(_config.imageDir.c_str());
    }

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
    if ((scroll < 0.0f || IsKeyDown(KEY_MINUS) || IsKeyPressed(KEY_S) || IsKeyPressedRepeat(KEY_S))
        && _camera.zoom > 0.5f
    ) {
        _camera.zoom -= zoomVal;
    }
    // "scroll up" or "+" or "W" to zoom in
    else if ((scroll > 0.0f || IsKeyDown(KEY_EQUAL) || IsKeyPressed(KEY_W) || IsKeyPressedRepeat(KEY_W))
        && _camera.zoom <= 100.0f
    ) {
        _camera.zoom += zoomVal;
    }
    // "0" or "Z" to reset zoom
    else if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_Z)) {
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
    // "R" to reset image
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

void ImageViewport::LoadFile(const char* filePath) {
    char* path = const_cast<char*>(filePath);
    FilePathList list{
        .capacity = 1,
        .count = 1,
        .paths = &path,
    };
    LoadFiles(list);
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
        // check if the file is png/jpg or not
        if (!utils::IsValidImage(path)) {
            continue;
        }

        _images.emplace_back(path);
    }

    _currentImageIdx = 0;
    CalcDstRectangle();
    // change image directories
    _config.SetImageDirs(GetCurrentImage().directory.c_str());

    if (_images.empty()) {
        logger::info("No images found!");
    }
}

void ImageViewport::LoadPath(const char* path) {
    CleanupImages();
    if (!_images.empty()) {
        _images.clear();
    }

    std::filesystem::path filesPath{ path };
    // std::filesystem::directory_iterator filesItr{ path };
    for (const auto& file : std::filesystem::directory_iterator{ filesPath }) {
        // check if the file is png/jpg or not
        if (!utils::IsValidImage(file.path().c_str())) {
            continue;
        }

        _images.emplace_back(file.path().c_str());
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

    float w = static_cast<float>(_config.windowWidth);
    float h = static_cast<float>(_config.windowHeight);
    const float winAspectRatio = w / h;

    if (GetCurrentImage().aspectRatio < winAspectRatio) {
        // keep the original size if the window is bigger than the image
        if (GetCurrentImage().texture.height < _config.windowHeight) {
            h = GetCurrentImage().texture.height;
        }
        _dstRectangle.x      = -h * 0.5f * GetCurrentImage().aspectRatio;
        _dstRectangle.y      = -h * 0.5f;
        _dstRectangle.width  = h * GetCurrentImage().aspectRatio;
        _dstRectangle.height = h;
    } else {
        // keep the original size if the window is bigger than the image
        if (GetCurrentImage().texture.width < _config.windowWidth) {
            w = GetCurrentImage().texture.width;
        }
        _dstRectangle.x      = -w * 0.5f;
        _dstRectangle.y      = -w * 0.5f * 1.0f / GetCurrentImage().aspectRatio;
        _dstRectangle.width  = w;
        _dstRectangle.height = w * 1.0f / GetCurrentImage().aspectRatio;
    }
}

void ImageViewport::DeleteImage() {
    if (_images.empty())
        return;

    if (!std::filesystem::exists(_config.trashDir)) {
        std::filesystem::create_directory(_config.trashDir);
    }

    // move the files to `.trash`
    logger::log("moving: \"%s\" to \"%s\"", GetCurrentImage().filepath.c_str(), _config.trashDir.c_str());
    std::filesystem::rename(GetCurrentImage().filepath, _config.trashDir + GetCurrentImage().filename);

    const std::string rawImageFileName = GetCurrentImage().filenameNoExt + _config.rawImageExt;
    const std::string rawImage = _config.rawImageDir + rawImageFileName;
    if (std::filesystem::exists(rawImage)) {
        logger::log("moving: \"%s\" to \"%s\"", rawImage.c_str(), _config.trashDir.c_str());
        std::filesystem::rename(rawImage, _config.trashDir + rawImageFileName);
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
