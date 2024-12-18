#ifndef MBED_SSD1306_DRAW2D_H
#define MBED_SSD1306_DRAW2D_H

#include "SSD1306.h"

namespace mbed_nuvoton {

class Draw2D
{
public:
    Draw2D(SSD1306 &lcd);
    
    void drawLine(int x1, int y1, int x2, int y2, int fgColor, int bgColor);
    void drawCircle(int xc, int yc, int r, int fgColor, int bgColor);
    void drawRectangle(int x0, int y0, int x1, int y1, int fgColor, int bgColor);
    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int fgColor, int bgColor);
    
protected:
    SSD1306 &LCD_2D;
};

}   /* namespace mbed_nuvoton */

#endif  /* MBED_SSD1306_DRAW2D_H */
