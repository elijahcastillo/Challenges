#include "utils.h"
#include <string.h>

void drawResponsiveText(const char *text, Rectangle container, float textWidthPercent, float textHeightPercent, float posXPercent, float posYPercent, float spacingPercent) {
    // Calculate the text height based on the container
    int textSize = container.height * textHeightPercent;
    int textLength = strlen(text);

    // Calculate total width of text with spacing
    Vector2 textSizeVec = MeasureTextEx(GetFontDefault(), text, textSize, spacingPercent * textSize);

    // Scale text width to fit within container width if needed
    if (textSizeVec.x > container.width * textWidthPercent) {
        textSize = (container.width * textWidthPercent) / ((textSizeVec.x / textSize) + spacingPercent * (textLength - 1));
        textSizeVec = MeasureTextEx(GetFontDefault(), text, textSize, spacingPercent * textSize);
    }

    // Calculate text position within the container
    float textPosX = container.x + container.width * posXPercent - textSizeVec.x / 2;
    float textPosY = container.y + container.height * posYPercent - textSizeVec.y / 2;

    // Draw the text with spacing
    DrawTextEx(GetFontDefault(), text, (Vector2){textPosX, textPosY}, textSize, spacingPercent * textSize, BLACK);
}
