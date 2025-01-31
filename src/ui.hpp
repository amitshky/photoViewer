#pragma once

#include "types.hpp"

namespace ui {

void InitUI();
void CleanupUI();
void BeginUI();
void EndUI();

void UnFocusAllWindows();

void ImageInfoWindow(const std::optional<ImageDetails>& imgInfo, bool show);

/**
  * @returns true if `Apply` button is pressed
  */
[[nodiscard]] bool PathsInputWindow(ImagePaths& paths, bool show);

} // namespace ui

