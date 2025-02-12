#include "imageViewport.hpp"

#include <filesystem>

#include "raylib.h"
#include "raylib/src/external/stb_image.h"

#include "logger.hpp"
#include "utils.hpp"
#include "timer.hpp"


ImageViewport::ImageViewport(const ImageViewportInfo& info)
    : _info{ info },
      _currentImageIdx{ 0 },
      _dstRectangle{ 0.0f, 0.0f, 0.0f, 0.0f },
      _camera{},
      _images{},
      _imageRotation{ ImageRotation::NONE },
      _originalRotation{ ImageRotation::NONE } {
    Init();
}

void ImageViewport::Init() {
   LoadImages(_info.imagePath);
}

void ImageViewport::Cleanup() {
    UnloadTexture(_texture);
}

void ImageViewport::Draw() {
    if (_images.empty())
        return;

    BeginMode2D(_camera);

    const Vector2 origin{ _dstRectangle.width / 2.0f, _dstRectangle.height / 2.0f };
    DrawTexturePro(
        _texture,
        _srcRectangle,
        _dstRectangle,
        origin,
        static_cast<float>(_imageRotation),
        WHITE
    );

    EndMode2D();
}

void ImageViewport::Resize(const uint64_t width, const uint64_t height) {
    _info.windowWidth = width;
    _info.windowHeight = height;

    _camera.offset = Vector2{
        static_cast<float>(width) * 0.5f,
        static_cast<float>(height) * 0.5f
    };

    CalcDstRectangle();
}

void ImageViewport::LoadImages(const char* path) {
    if (std::filesystem::is_directory(path)) {
        LoadFilesFromDir(path);
    } else if (std::filesystem::is_regular_file(path)) {
        LoadFile(path);
    }
}

void ImageViewport::LoadFile(const char* filePath) {
    char* path = const_cast<char*>(filePath);
    FilePathList list{
        .capacity = 1,
        .count = 1,
        .paths = &path,
    };
    LoadFilesFromList(list);
}

void ImageViewport::LoadFilesFromList(const FilePathList& files) {
    if (files.count <= 0)
        return;

    Cleanup();
    if (!_images.empty()) {
        _images.clear();
    }

    _images.reserve(files.count);
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
    UpdateImagePath(GetDirectoryPath(files.paths[0]));

    if (_images.empty()) {
        logger::info("No images found!");
    } else {
        LoadCurrentImage();
    }

    Reset();
}

void ImageViewport::LoadFilesFromDir(const char* path) {
    Cleanup();
    if (!_images.empty()) {
        _images.clear();
    }

    const std::filesystem::path filesPath{ path };
    _images.reserve(std::distance(
        std::filesystem::directory_iterator(filesPath),
        std::filesystem::directory_iterator()
    ));

    for (const auto& file : std::filesystem::directory_iterator{ filesPath }) {
        // in windows path().c_str() gives wide char
        const std::string fpath = file.path().string();
        // check if the file is png/jpg or not
        if (!utils::IsValidImage(fpath.c_str())) {
            continue;
        }

        _images.emplace_back(fpath.c_str());
    }

    _currentImageIdx = 0;
    CalcDstRectangle();

    if (_images.empty()) {
        logger::info("No images found!");
    } else {
        LoadCurrentImage();
    }

    Reset();
}

void ImageViewport::ZoomIn() {
    if (_camera.zoom > 100.0f)
        return;

    _camera.zoom += _zoomVal;
}

void ImageViewport::ZoomOut() {
    if (_camera.zoom <= 0.5f)
        return;

    _camera.zoom -= _zoomVal;
}

void ImageViewport::ResetZoom() {
    _camera.zoom = 1.0f;
}

void ImageViewport::RotateCW() {
    _imageRotation = 
        static_cast<ImageRotation>((static_cast<const uint32_t>(_imageRotation)
        + _rotationVal) % 360);
    CalcDstRectangle();
}

void ImageViewport::RotateCCW() {
    _imageRotation = 
        static_cast<ImageRotation>((static_cast<const uint32_t>(_imageRotation)
        + 3 * _rotationVal) % 360); // subtracting _rotationVal was messing with
                                    // image orientation (the value would be 
                                    // negative) so _rotationVal is multiplied by 3 and
                                    // added, which gives the same effect
    CalcDstRectangle();
}

void ImageViewport::Reset() {
    _camera.offset = Vector2{
        .x = static_cast<float>(_info.windowWidth) * 0.5f,
        .y = static_cast<float>(_info.windowHeight) * 0.5f
    };
    _camera.target = Vector2{ 0.0f, 0.0f };
    _camera.rotation = 0.0f;
    _camera.zoom = 1.0f;
    _imageRotation = _originalRotation;
    CalcDstRectangle();
}

void ImageViewport::NextImage() {
    if (_currentImageIdx + 1 >= _images.size())
        return;

    ++_currentImageIdx;
    LoadCurrentImage();
}

void ImageViewport::PrevImage() {
    if (_currentImageIdx - 1 < 0)
        return;

    --_currentImageIdx;
    LoadCurrentImage();
}

void ImageViewport::FirstImage() {
    if (_images.empty())
        return;
 
    _currentImageIdx = 0;
    LoadCurrentImage();
}

void ImageViewport::LastImage() {
    if (_images.empty())
        return;

    _currentImageIdx = _images.size() - 1;
    LoadCurrentImage();
}

void ImageViewport::DeleteImage() {
    // TODO: switch to next or prev or no image after deleting
    if (_images.empty())
        return;

    if (!std::filesystem::exists(_info.trashDir)) {
        std::filesystem::create_directory(_info.trashDir);
    }

    // move the files to the `trash directory`
    logger::log("moving: \"%s\" to \"%s\"",
        GetCurrentImage().filepath.c_str(), _info.trashDir);
    std::filesystem::rename(
        GetCurrentImage().filepath,
        _info.trashDir + GetCurrentImage().filename
    );

    const std::string rawImageFileName =
        GetCurrentImage().filenameNoExt + _info.rawImageExt;
    const std::string rawImage = _info.rawImagePath + rawImageFileName;

    if (std::filesystem::exists(rawImage)) {
        logger::log("moving: \"%s\" to \"%s\"", rawImage.c_str(), _info.trashDir);
        std::filesystem::rename(rawImage, _info.trashDir + rawImageFileName);
    }

    _images.erase(_images.begin() + _currentImageIdx);

    if (_currentImageIdx - 1 >= 0) {
        --_currentImageIdx;
    } else {
        _currentImageIdx = 0;
    }

    CalcDstRectangle();
}

void ImageViewport::MoveCameraUsingMouse() {
    const Vector2 delta = GetMouseDelta();
    _camera.target.x -= delta.x / _camera.zoom;
    _camera.target.y -= delta.y / _camera.zoom;
}

void ImageViewport::CalcDstRectangle() {
    if (_images.empty())
        return;

    float w = static_cast<float>(_info.windowWidth);
    float h = static_cast<float>(_info.windowHeight);
    const float winAspectRatio = w / h;

    if (_aspectRatio < winAspectRatio) {
        // keep the original size if the window is bigger than the image
        if (_texture.height < _info.windowHeight) {
            h = _texture.height;
        }
        _dstRectangle.width  = h * _aspectRatio;
        _dstRectangle.height = h;
    } else {
        // keep the original size if the window is bigger than the image
        if (_texture.width < _info.windowWidth) {
            w = _texture.width;
        }
        _dstRectangle.width  = w;
        _dstRectangle.height = w * 1.0f / _aspectRatio;
    }

    // reduce the dstRectangle width or height based on how the image is rotated
    // so that it fits the window
    // FIXME: resizing window, doesnt fit images to window sometimes
    if (_imageRotation == ImageRotation::RIGHT_270
        || _imageRotation == ImageRotation::RIGHT_90) {
        // when rotated 90 degree, the width of the dstRectangle becomes
        // the height and vice-versa
        if (_dstRectangle.width > _info.windowHeight) {
            _dstRectangle.width = _info.windowHeight;
            _dstRectangle.height = (1.0f / _aspectRatio) * _dstRectangle.width;
        } else if (_dstRectangle.height > _info.windowWidth) {
            _dstRectangle.height = _info.windowWidth;
            _dstRectangle.width = _aspectRatio * _dstRectangle.height;
        }
    }
}

void ImageViewport::LoadCurrentImage() {
    Timer t{ "Loading image \"" + GetCurrentImage().filepath + '"' };

    // TODO: load image directly using stbi_load
    // read file
    FILE* file;
    unsigned char* imageData = nullptr; // image data
    uint64_t imageDataSize = 0;
    const char* filepath = GetCurrentImage().filepath.c_str();
    file = fopen(filepath, "rb");
    if (file == nullptr) {
        logger::error("Failed to load file: %s", filepath);
        imageDataSize = 0;
        // TODO: handle failed to load (show a toast msg)
        std::exit(-1);
    }

    fseek(file, 0, SEEK_END);
    imageDataSize = ftell(file);
    rewind(file);
    imageData = new unsigned char[imageDataSize];
    if (fread(imageData, sizeof(unsigned char), imageDataSize, file) != imageDataSize) {
        logger::error("Failed to read file: %s", filepath);
        delete[] imageData;
        imageDataSize = 0;
        // TODO: handle failed to load (show a toast msg)
        std::exit(-1);
    }

    fclose(file);

    int comp = 0; // image components (R, G, B, A)
    Image image{};

    image.data = stbi_load_from_memory(
        imageData,
        imageDataSize,
        &image.width,
        &image.height,
        &comp,
        0
    );

    if (image.data == nullptr)
        return; // TODO: handle this case

    image.mipmaps = 1;
    if (comp == 1) {
        image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
    } else if (comp == 2) {
        image.format = PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;
    } else if (comp == 3) {
        image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    } else if (comp == 4) {
        image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    }

    // reset image rotation and
    // change it later if orientation (exif data) of image is not '1'
    _originalRotation = ImageRotation::NONE;
    _imageRotation = ImageRotation::NONE;

    tinyexif::EXIFInfo exifInfo;
    const int errCode = exifInfo.parseFrom(
        const_cast<const unsigned char*>(imageData),
        imageDataSize
    );

    // check for error when parsing EXIF data
    if (errCode == PARSE_EXIF_ERROR_NO_EXIF) {
        logger::info("EXIF data not found!");
    } else if (errCode == PARSE_EXIF_ERROR_NO_JPEG) {
        logger::warn("Cannot parse EXIF data for non-JPEG images!");
    } else if (errCode == PARSE_EXIF_ERROR_UNKNOWN_BYTEALIGN) {
        logger::error("Error reading EXIF data (UNKNOWN BYTE ALIGNMENT)!");
    } else if (errCode == PARSE_EXIF_ERROR_CORRUPT) {
        logger::error("Error reading EXIF data (DATA CORRUPTED)!");
    } else {
        // no error
        GetCurrentImage().exifInfo = exifInfo;
        // rotate the images if the orientation is not correct
        // ref: https://jdhao.github.io/2019/07/31/image_rotation_exif_info/
        if (exifInfo.Orientation == 8) {
            _originalRotation = ImageRotation::RIGHT_270;
            _imageRotation = _originalRotation;
        } else if (exifInfo.Orientation == 3) {
            _originalRotation = ImageRotation::RIGHT_180;
            _imageRotation = _originalRotation;
        } else if (exifInfo.Orientation == 6) {
            _originalRotation = ImageRotation::RIGHT_90;
            _imageRotation = _originalRotation;
        }

        CalcDstRectangle();
    }

    _texture = LoadTextureFromImage(image);
    _aspectRatio =
        static_cast<float>(image.width) / static_cast<float>(image.height);
    _srcRectangle = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(image.width),
        .height = static_cast<float>(image.height),
    };

    delete[] imageData;
    UnloadImage(image);
    CalcDstRectangle();
}
