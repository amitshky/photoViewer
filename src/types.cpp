#include "types.hpp"

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
        imageDir = path;
    else
        imageDir = std::string{ path } + '/';

    rawImageDir = imageDir;
    trashDir    = imageDir + "trash/";
}

void Config::SetImageDirs(const char* imgDir, const char* rawImgDir, const char* trDir) {
    int64_t len = strlen(imgDir);
    if (imgDir[len - 1] == '/' )
        imageDir = imgDir;
    else
        imageDir = std::string{ imgDir } + '/';

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
      filenameNoExt{ "" } {
    // const Image imgData = LoadImage(path);
    // texture = LoadTextureFromImage(imgData);
    // aspectRatio = static_cast<float>(imgData.width) / static_cast<float>(imgData.height);
    // srcRectangle = {
    //     .x = 0.0f,
    //     .y = 0.0f,
    //     .width = static_cast<float>(imgData.width),
    //     .height = static_cast<float>(imgData.height),
    // };
    // UnloadImage(imgData);

    filename = GetFileName(path);
    for (const auto& ch : filename) {
        if (ch == '.')
            break;

        filenameNoExt += ch;
    }
}
