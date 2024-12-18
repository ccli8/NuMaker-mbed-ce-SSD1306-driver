#include <mbed.h>
#include "draw2D.h"
#include "ssd1306.h"

namespace mbed_nuvoton {

Draw2D::Draw2D(SSD1306 &LCD_2D)
    : LCD_2D(LCD_2D)
{
}

// draw Bresenham Line
void Draw2D::drawLine(int x1, int y1, int x2, int y2, int fgColor, int bgColor)
{
    int dy = y2 - y1;
    int dx = x2 - x1;
    int stepx, stepy;

    if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
    dy <<= 1;        // dy is now 2*dy
    dx <<= 1;        // dx is now 2*dx

    LCD_2D.drawPixel(x1,y1, fgColor, bgColor);
    if (dx > dy) 
    {
        int fraction = dy - (dx >> 1);  // same as 2*dy - dx
        while (x1 != x2) 
        {
           if (fraction >= 0) 
           {
               y1 += stepy;
               fraction -= dx;          // same as fraction -= 2*dx
           }
           x1 += stepx;
           fraction += dy;              // same as fraction -= 2*dy
           LCD_2D.drawPixel(x1, y1, fgColor, bgColor);
        }
     } else {
        int fraction = dx - (dy >> 1);
        while (y1 != y2) {
           if (fraction >= 0) {
               x1 += stepx;
               fraction -= dy;
           }
           y1 += stepy;
           fraction += dx;
           LCD_2D.drawPixel(x1, y1, fgColor, bgColor);
        }
     }
}

// draw Bresenham Circle
void Draw2D::drawCircle(int xc, int yc, int r, int fgColor, int bgColor)
{
    int x = 0; 
    int y = r; 
    int p = 3 - 2 * r;
    if (!r) return;     
    while (y >= x) // only formulate 1/8 of circle
    {
        LCD_2D.drawPixel(xc-x, yc-y, fgColor, bgColor);//upper left left
        LCD_2D.drawPixel(xc-y, yc-x, fgColor, bgColor);//upper upper left
        LCD_2D.drawPixel(xc+y, yc-x, fgColor, bgColor);//upper upper right
        LCD_2D.drawPixel(xc+x, yc-y, fgColor, bgColor);//upper right right
        LCD_2D.drawPixel(xc-x, yc+y, fgColor, bgColor);//lower left left
        LCD_2D.drawPixel(xc-y, yc+x, fgColor, bgColor);//lower lower left
        LCD_2D.drawPixel(xc+y, yc+x, fgColor, bgColor);//lower lower right
        LCD_2D.drawPixel(xc+x, yc+y, fgColor, bgColor);//lower right right
        if (p < 0) p += 4*(x++) + 6; 
        else p += 4*((x++) - y--) + 10; 
     } 
}

void Draw2D::drawRectangle(int x0, int y0, int x1, int y1, int fgColor, int bgColor)
{
    int x,y, tmp;
    if (x0>x1) { tmp = x1; x1 = x0; x0 = tmp; }
    if (y0>y1) { tmp = y1; y1 = y0; y0 = tmp; }
    for (x=x0; x<=x1; x++) LCD_2D.drawPixel(x,y0,fgColor, bgColor);
    for (y=y0; y<=y1; y++) LCD_2D.drawPixel(x0,y,fgColor, bgColor);
    for (x=x0; x<=x1; x++) LCD_2D.drawPixel(x,y1,fgColor, bgColor);
    for (y=y0; y<=y1; y++) LCD_2D.drawPixel(x1,y,fgColor, bgColor);       
}

void Draw2D::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int fgColor, int bgColor)
{
    drawLine(x0, y0, x1, y1, fgColor, bgColor);
    drawLine(x1, y1, x2, y2, fgColor, bgColor);
    drawLine(x0, y0, x2, y2, fgColor, bgColor);
}

}   /* namespace mbed_nuvoton */
