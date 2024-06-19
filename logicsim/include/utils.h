#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"

// // Function to draw responsive text
// void drawResponsiveText(const char *text, Rectangle container, float textWidthPercent, float textHeightPercent, float posXPercent, float posYPercent) {
//     // Calculate the text width and height based on the container
//     int textSize = container.height * textHeightPercent;
//     Vector2 textSizeVec = MeasureTextEx(GetFontDefault(), text, textSize, 1);
//
//     // Scale text width to fit within container width if needed
//     if (textSizeVec.x > container.width * textWidthPercent) {
//         textSize = container.width * textWidthPercent / (textSizeVec.x / textSize);
//         textSizeVec = MeasureTextEx(GetFontDefault(), text, textSize, 1);
//     }
//
//     // Calculate text position within the container
//     float textPosX = container.x + container.width * posXPercent - textSizeVec.x / 2;
//     float textPosY = container.y + container.height * posYPercent - textSizeVec.y / 2;
//
//     // Draw the text
//     DrawTextEx(GetFontDefault(), text, (Vector2){textPosX, textPosY}, textSize, 1, BLACK);
// }

void drawResponsiveText(const char *text, Rectangle container, float textWidthPercent, float textHeightPercent, float posXPercent, float posYPercent, float spacingPercent);

#endif // UTILS_H
