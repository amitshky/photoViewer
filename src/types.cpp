#include "types.hpp"

#include "cctype"
#include "logger.hpp"
#include "timer.hpp"

Config::Config(const char* path,
    const char* rawExt,
    const uint64_t wWidth,
    const uint64_t wHeight)
    : rawImageExt{ rawExt },
      windowWidth{ wWidth },
      windowHeight{ wHeight } {
    SetImageDirs(path);
}

void Config::SetImageDirs(const char* path) {
    const int64_t len = strlen(path);
    if (path[len - 1] == '/' )
        imagePath = path;
    else
        imagePath = std::string{ path } + '/';

    rawImageDir = imagePath;
    trashDir    = imagePath + "trash/";
}

void Config::SetImageDirs(const char* imgDir, const char* rawImgDir, const char* trDir) {
    int64_t len = strlen(imgDir);
    if (imgDir[len - 1] == '/' )
        imagePath = imgDir;
    else
        imagePath = std::string{ imgDir } + '/';

    len = strlen(rawImgDir);
    if (rawImgDir[len - 1] == '/' )
        rawImageDir = rawImgDir;
    else
        rawImageDir = std::string{ rawImgDir } + '/';

    len = strlen(trDir);
    if (trDir[len - 1] == '/' )
        trashDir = trDir;
    else
        trashDir = std::string{ trDir } + '/';
}


ImageDetails::ImageDetails(const char* path)
    : filepath{ path },
      filename{ "" },
      filenameNoExt{ "" },
      extension{ "" },
      data{ nullptr } {
    FILE* file;
    {
        Timer tt{ "File Read" };

        file = fopen(path, "rb");
        if (file == nullptr) {
            logger::error("Failed to load file: %s", path);
            // TODO: handle failed to load
            std::exit(-1);
        }

        fseek(file, 0, SEEK_END);
        dataSize = ftell(file);
        rewind(file);
        data = new unsigned char[dataSize];
        if (fread(data, sizeof(unsigned char), dataSize, file) != dataSize) {
            logger::error("Failed to read file: %s", path);
            delete[] data;
            // TODO: handle failed to load
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

// TODO: check if these implementations is a proper move constructor and operator or not
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
