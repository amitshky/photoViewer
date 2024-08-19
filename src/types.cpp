#include "types.hpp"

ImageDetails::ImageDetails(const char* path)
    : filepath{ path },
      directory{ GetDirectoryPath(path) },
      fileExt{GetFileExtension(path)},
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
