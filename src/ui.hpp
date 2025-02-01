#pragma once

#include <functional>
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
  * @param `fnOnApply` - called when 'apply' button is pressed (or CTRL+ENTER)
  * @param `fnOnLoadFiles` - called when 'load files' button is pressed (or SHIFT+ENTER)
  * 
  * @returns ImGuiWindow handle
  */
ImGuiWindow* CreateConfigWindow(
    TextFields& paths,
    bool show,
    std::function<void(void)> fnOnApply,
    std::function<void(void)> fnOnLoadFiles
);

} // namespace ui

