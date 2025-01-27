#pragma once

#include "types.hpp"

namespace ui {

void InitUI();
void CleanupUI();
void BeginUI();
void EndUI();

void UnFocusAllWindows();

void ImageInfoWindow(const ImageDetails& imgInfo, bool show);

/**
  * @returns true if `Apply` button is pressed
  */
[[nodiscard]] bool PathInputWindow(ImagePaths& paths);

} // namespace ui

