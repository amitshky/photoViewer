#pragma once

#include "imgui/imgui_internal.h"
#include "types.hpp"

namespace ui {

void InitUI();
void CleanupUI();
void BeginUI();
void EndUI();

void UnFocusAllWindows();

ImGuiWindow* CreateImageInfoWindow(
    const std::optional<ImageDetails>& imgInfo,
    bool show
);

/**
  * Press CTRL+ENTER when window is in focus to trigger `apply`.
  * Press SHIFT+ENTER when window is in focus to trigger `load files`.
  * 
  * @param `paths` - gets updated when the input text fields are updated
  * @param `show` - to show/hide the window
  * @param `isApplyTriggered` - is set to true if apply button is pressed
  * @param `isLoadTriggered` - is set to true if load files button is pressed
  * 
  * @returns ImGuiWindow handle
  */
ImGuiWindow* CreatePathInputWindow(
    ImagePaths& paths,
    bool show,
    bool& isApplyTriggered,
    bool& isLoadTriggered
);

} // namespace ui

