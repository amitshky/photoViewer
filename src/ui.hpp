#pragma once

#include "types.hpp"

void InitUI();
void CleanupUI();
void BeginUI();
void EndUI();

void ImageInfoWindow(const ImageDetails& imgInfo, bool show);
