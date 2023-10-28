#include <stdio.h>
#include "custom_raygui.h"

void GuiTooltipCustom(Rectangle controlRec, char* text) {
    Vector2 textSize = MeasureTextEx(GuiGetFont(), text, (float)GuiGetStyle(DEFAULT, TEXT_SIZE), (float)GuiGetStyle(DEFAULT, TEXT_SPACING));

    if ((controlRec.x + textSize.x + 16) > GetScreenWidth()) controlRec.x -= (textSize.x + 16 - controlRec.width);

    GuiPanel((Rectangle){ controlRec.x + controlRec.width + 4, controlRec.y, textSize.x + 16, GuiGetStyle(DEFAULT, TEXT_SIZE) + 8.f }, NULL);

    int textPadding = GuiGetStyle(LABEL, TEXT_PADDING);
    int textAlignment = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
    GuiSetStyle(LABEL, TEXT_PADDING, 0);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiLabel((Rectangle){ controlRec.x + controlRec.width + 4, controlRec.y, textSize.x + 16, GuiGetStyle(DEFAULT, TEXT_SIZE) + 8.f }, text);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, textAlignment);
    GuiSetStyle(LABEL, TEXT_PADDING, textPadding);
}
