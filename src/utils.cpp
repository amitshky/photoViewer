#include "utils.hpp"

#include <string>
#include "raylib.h"

namespace utils {

bool IsImage(const char* path) {
    const std::string ext = GetFileExtension(path);
    if (ext != ".png"  &&
        ext != ".PNG"  &&
        ext != ".jpg"  &&
        ext != ".JPG"  &&
        ext != ".jpeg" &&
        ext != ".JPEG"
    ) {
        return true;
    }

    return false;
}

}
