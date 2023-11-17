#ifndef W0RMNET_TEXT_RECTANGLE_BOUNDS_H
#define W0RMNET_TEXT_RECTANGLE_BOUNDS_H

#include "raylib.h"

void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);   // Draw text using font inside rectangle limits
void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);    // Draw text using font inside rectangle limits with support for text selection

#endif //W0RMNET_TEXT_RECTANGLE_BOUNDS_H
