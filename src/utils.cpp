#include "utils.hpp"

void CalcDstImageAspects(Rectangle& imgDstRec, const float imgAspectRatio, const float winAspectRatio, const float width, const float height) {
    if (imgAspectRatio < winAspectRatio) {
        imgDstRec.x = -static_cast<float>(height) * 0.5f * imgAspectRatio;
        imgDstRec.y = -static_cast<float>(height) * 0.5f;
        imgDstRec.width = static_cast<float>(height) * imgAspectRatio;
        imgDstRec.height = static_cast<float>(height);
    } else {
        imgDstRec.x = -static_cast<float>(width) * 0.5f;
        imgDstRec.y = -static_cast<float>(width) * 0.5f * 1.0f / imgAspectRatio;
        imgDstRec.width = static_cast<float>(width);
        imgDstRec.height = static_cast<float>(width) * 1.0f / imgAspectRatio;
    }
}
