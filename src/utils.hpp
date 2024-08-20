#pragma once

#include "types.hpp"

namespace utils {

void ParseArgs(int argc, char* argv[], Config& config);

// check if the file path is a valid image
bool IsValidImage(const char* filePath);

}
