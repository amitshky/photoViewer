#include "types.hpp"

#include <filesystem>

#include "logger.hpp"
#include "timer.hpp"

Config::Config(const char* path,
    const char* rawExt,
    const uint64_t wWidth,
    const uint64_t wHeight)
    : rawImageExt{ rawExt },
      windowWidth{ wWidth },
      windowHeight{ wHeight } {
    InitImageDirs(path);
}

void Config::InitImageDirs(const char* path) {
    // if no path is specified, set it as current path
    std::string imgDir{ path };
    if (imgDir.empty()) {
        imgDir = std::filesystem::current_path().string();
    }

    if (imgDir[imgDir.length() - 1] == '/' ) {
        imagePath = imgDir;
    } else {
        imagePath = imgDir + '/';
    }

    rawImagePath = imagePath;
    trashDir = imagePath + "trash/";
}


ImageDetails::ImageDetails(const char* path)
    : filepath{ path },
      filename{ "" },
      filenameNoExt{ "" },
      extension{ "" },
      data{ nullptr } {
    FILE* file;
    {
        Timer tt{ "Reading file \"" + filepath + '"' };

        file = fopen(path, "rb");
        if (file == nullptr) {
            logger::error("Failed to load file: %s", path);
            // TODO: handle failed to load (show a toast msg)
            std::exit(-1);
        }

        fseek(file, 0, SEEK_END);
        dataSize = ftell(file);
        rewind(file);
        data = new unsigned char[dataSize];
        if (fread(data, sizeof(unsigned char), dataSize, file) != dataSize) {
            logger::error("Failed to read file: %s", path);
            delete[] data;
            // TODO: handle failed to load (show a toast msg)
            std::exit(-1);
        }

        fclose(file);
    }

    filename = GetFileName(path);
    extension = GetFileExtension(filename.c_str());
    for (const auto& ch : filename) {
        if (ch == '.')
            break;

        filenameNoExt += ch;
    }
}

ImageDetails::~ImageDetails() {
    delete[] data;
    data = nullptr;
    dataSize = 0;
}

ImageDetails::ImageDetails(const ImageDetails& other) {
    filepath = other.filepath;
    filename = other.filename;
    filenameNoExt = other.filenameNoExt;

    data = new unsigned char[other.dataSize];
    memcpy(data, other.data, other.dataSize);

    dataSize = other.dataSize;
}

ImageDetails& ImageDetails::operator=(const ImageDetails& other) {
    if (&other == this) {
        return *this;
    }

    filepath = other.filepath;
    filename = other.filename;
    filenameNoExt = other.filenameNoExt;

    data = new unsigned char[other.dataSize];
    memcpy(data, other.data, other.dataSize);

    dataSize = other.dataSize;

    return *this;
}

ImageDetails::ImageDetails(ImageDetails&& other) {
    filepath = std::move(other.filepath);
    filename = std::move(other.filename);
    filenameNoExt = std::move(other.filenameNoExt);
    data = other.data;;
    dataSize = other.dataSize;
    other.data = nullptr;
    other.dataSize = 0;
}

ImageDetails& ImageDetails::operator=(ImageDetails&& other) {
    if (&other == this) {
        return *this;
    }

    filepath = std::move(other.filepath);
    filename = std::move(other.filename);
    filenameNoExt = std::move(other.filenameNoExt);
    data = other.data;;
    dataSize = other.dataSize;
    other.data = nullptr;
    other.dataSize = 0;

    return *this;
}
