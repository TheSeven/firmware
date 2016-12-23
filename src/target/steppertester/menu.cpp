#include "global.h"
#include "menu.h"
#include "lcd.h"
#include "font.h"
#include "button.h"


int showMenu(const Menu* menu)
{
    int offset = 0;
    int selected = 0;
    while (true)
    {
        int row = menu->startRow;
        int height = menu->height;
        lcd.clearRows(row, height);
        lcd.print(row++, 0, font, 0x80, menu->title);
        int index = offset;
        while (--height)
        {
            lcd.print(row, 0, font, index == selected ? 0xff : 0, menu->entry[index].text);
            if (menu->entry[index].renderer) menu->entry[index].renderer(menu->entry[index].arg, row);
            row++;
            if (++index >= menu->entryCount) break;
        }
        uint32_t buttons = waitButton(menu->timeout << 20);
        if (buttons & 1)
        {
            if (menu->entry[selected].handler)
            {
                int row = menu->startRow + 1 + selected - offset;
                lcd.print(row, 0, font, 0, menu->entry[selected].text);
                int result = menu->entry[selected].handler(menu->entry[selected].arg, row);
                if (result < -1) return result;
            }
            else return selected;
        }
        else if (buttons & 2)
        {
            if (!selected);
            else if (--selected < offset) offset = selected;
        }
        else if (buttons & 4)
        {
            if (selected >= menu->entryCount - 1);
            else if (++selected >= offset + menu->height - 1) offset = selected - menu->height + 2;
        }
        else return (buttons & 8) ? -1 : -2;
    }
}
