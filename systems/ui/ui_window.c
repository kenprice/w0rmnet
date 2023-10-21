#include "ui_window.h"

int update_window(WindowStruct* window) {
    Vector2 mousePosition = GetMousePosition();
    // Update window drag
    if (window->supportDrag) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Window can be dragged from the top window bar
            if (CheckCollisionPointRec(mousePosition, (Rectangle) {window->windowBounds.x, window->windowBounds.y,
                                                                   (float) window->windowBounds.width,
                                                                   TITLEBAR_HEIGHT})) {
                window->dragMode = true;
                window->panOffset.x = mousePosition.x - window->windowBounds.x;
                window->panOffset.y = mousePosition.y - window->windowBounds.y;
            }
        }

        if (window->dragMode) {
            window->windowBounds.x = (mousePosition.x - window->panOffset.x);
            window->windowBounds.y = (mousePosition.y - window->panOffset.y);

            // Check screen limits to avoid moving out of screen
            if (window->windowBounds.x < 0) window->windowBounds.x = 0;
            else if (window->windowBounds.x > (GetScreenWidth() - window->windowBounds.width)) window->windowBounds.x =
                                                                                                       GetScreenWidth() -
                                                                                                       window->windowBounds.width;

            if (window->windowBounds.y < 0) window->windowBounds.y = 0;
            else if (window->windowBounds.y > (GetScreenHeight() - window->windowBounds.height)) window->windowBounds.y =
                                                                                                         GetScreenHeight() -
                                                                                                         window->windowBounds.height;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) window->dragMode = false;
        }
    }

    return CheckCollisionPointRec(mousePosition, window->windowBounds);
}
