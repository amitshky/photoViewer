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

ImageDetails::ImageDetails(const char* path)
    : filepath{ path },
      directory{ std::string{ GetDirectoryPath(path) } + '/' },
      filenameNoExt{ "" } {
    const Image imgData = LoadImage(path);

    texture = LoadTextureFromImage(imgData);
    aspectRatio = static_cast<float>(imgData.width) / static_cast<float>(imgData.height);
    srcRectangle = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(imgData.width),
        .height = static_cast<float>(imgData.height),
    };

    UnloadImage(imgData);

    // FIXME: this can break if the path contains dot in directory name
    filename = GetFileName(path);
    for (const auto& ch : filename) {
        if (ch == '.')
            break;

        filenameNoExt += ch;
    }
}
