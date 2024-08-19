#include "imageViewport.hpp"

#include <filesystem>
#include "logger.hpp"
#include "raylib.h"
#include "utils.hpp"

ImageViewport::ImageViewport(const char* path, const uint64_t width, const uint64_t height) {
    // LoadImages();
    // LoadImagesFromDirectory();

    const std::filesystem::directory_iterator dirItr{ path };
    for (const auto& file : dirItr) {
        if (std::filesystem::is_directory(file)) {
            continue;
        } else if (!std::filesystem::is_regular_file(file)) {
            continue;
        }
        // check if the file is png/jpg or not
        const char* path = file.path().c_str();
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

    // TODO: temporary (remove this later when you can open the app on its own without needing an image in the directory)
    if (_images.size() == 0) {
        logger::error("No images found in the current directory!");
        CloseWindow();
        std::exit(-1);
    }

    _currentImageIdx = 0;
    _dstRectangle = { 0.0f, 0.0f, 0.0f, 0.0f };
    const float winAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    CalcDstImageAspects(_dstRectangle, _images[_currentImageIdx].aspectRatio, winAspectRatio, width, height);

    _camera = {
        .offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f },
        .target = Vector2{ 0.0f, 0.0f },
        .rotation = 0.0f,
        .zoom = 1.0f,
    };
} 

void ImageViewport::Display() {
    BeginMode2D(_camera);

    if (_images.size() > 0) {
        // TODO: do something when there are no images
        DrawTexturePro(_images[_currentImageIdx].texture,
            _images[_currentImageIdx].srcRectangle,
            _dstRectangle, 
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }

    EndMode2D();
}

void ImageViewport::CleanupImages() {
    for (const auto& img : _images) {
        UnloadTexture(img.texture);
    }
}

void ImageViewport::ProcessKeybindings(uint64_t& width,
        uint64_t& height,
        float& winAspectRatio,
        const std::string& rawFilePath) {
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
        _camera.offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f };
        _camera.target = Vector2{ 0.0f, 0.0f };
        _camera.rotation = 0.0f;
        _camera.zoom = 1.0f;
    } else if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && _currentImageIdx + 1 < _images.size()) { // "D" or "Right arrow" to view next image
        ++_currentImageIdx;
        CalcDstImageAspects(_dstRectangle, _images[_currentImageIdx].aspectRatio, winAspectRatio, width, height);
    } else if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && _currentImageIdx - 1 >= 0) { // "A" or "Left arrow" to view previous image
        --_currentImageIdx;
        CalcDstImageAspects(_dstRectangle, _images[_currentImageIdx].aspectRatio, winAspectRatio, width, height);
    } else if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_X)) { // "Delete" or "X" to delete image as well as raw image (if exists)
        const char* trash = "test/trash/";
        // TODO: do not hardcode
        if (!std::filesystem::exists(trash)) {
            std::filesystem::create_directory(trash);
        }

        std::filesystem::path imagePath{ _images[_currentImageIdx].filepath };
        std::filesystem::path rawImagePath{ rawFilePath + _images[_currentImageIdx].filenameNoExt + ".ARW" };

        // move the files to `.trash`
        std::filesystem::rename(imagePath, trash + _images[_currentImageIdx].filename);
        logger::log("deleting: \"%s\"", imagePath.c_str());
        if (std::filesystem::exists(rawImagePath)) {
            logger::log("deleting: \"%s\"", rawImagePath.c_str());
            std::filesystem::rename(rawImagePath, trash + _images[_currentImageIdx].filenameNoExt + ".ARW");
        }

        // FIXME: after deleting, the next image's aspect ratio is not loading correctly
        _images.erase(_images.begin() + _currentImageIdx);
        if (_currentImageIdx - 1 >= 0) {
            --_currentImageIdx;
        } else {
            _currentImageIdx = 0;
        }
        CalcDstImageAspects(_dstRectangle, _images[_currentImageIdx].aspectRatio, winAspectRatio, width, height);
    }
}

void ImageViewport::Resize(uint64_t width, uint64_t height, float winAspectRatio) {
    _camera.offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f };
    CalcDstImageAspects(_dstRectangle, _images[_currentImageIdx].aspectRatio, winAspectRatio, width, height);
}

void ImageViewport::LoadFiles(const FilePathList& files) {
    if (files.count > 0) {
        CleanupImages();
        _images.clear();
        _images.reserve(files.count);

        for (uint64_t i = 0; i < files.count; ++i) {
            _images.push_back(ImageDetails{ files.paths[i] });
        }

        _currentImageIdx = 0;
    }
}
