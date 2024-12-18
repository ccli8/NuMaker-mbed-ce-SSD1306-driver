// SSD1306Z LCD Driver: 0.96" lcd LY096BG30
#include "ssd1306.h"
#include "Font8x16.h"
#include "Font5x7.h"

namespace mbed_nuvoton {

void SSD1306::lcdWriteCommand(uint8_t lcd_Command)
{
    char data[2];
    data[0]=0x00;
    data[1]=lcd_Command;
    i2c.write(slave_addr, data, 2, 0);
}

void SSD1306::lcdWriteData(uint8_t lcd_Data)
{
    char data[2];
    data[0]=0x40;
    data[1]=lcd_Data;
    i2c.write(slave_addr, data, 2, 0);
}

void SSD1306::lcdSetAddr(uint8_t column, uint8_t page)
{
    lcdWriteCommand(0xb0+page);                     // set page address
    lcdWriteCommand(0x10 | ((column & 0xf0) >> 4)); // set column address MSB
    lcdWriteCommand(0x00 |  (column & 0x0f)      ); // set column address LSB
}

SSD1306::SSD1306(PinName sda, PinName scl, int address, int hz)
    : i2c(sda, scl),
    slave_addr(address)
{
    i2c.frequency(hz);
}

void SSD1306::initialize(void)
{
    lcdWriteCommand(0xae); //display off
    lcdWriteCommand(0x20);  //Set Memory Addressing Mode    
    lcdWriteCommand(0x10);  //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    lcdWriteCommand(0xb0);  //Set Page Start Address for Page Addressing Mode,0-7
    lcdWriteCommand(0xc8);  //Set COM Output Scan Direction
    lcdWriteCommand(0x00);//---set low column address
    lcdWriteCommand(0x10);//---set high column address
    lcdWriteCommand(0x40);//--set start line address
    lcdWriteCommand(0x81);//--set contrast control register
    lcdWriteCommand(0x7f);
    lcdWriteCommand(0xa1);//--set segment re-map 0 to 127
    lcdWriteCommand(0xa6);//--set normal display
    lcdWriteCommand(0xa8);//--set multiplex ratio(1 to 64)
    lcdWriteCommand(0x3F);//
    lcdWriteCommand(0xa4);//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    lcdWriteCommand(0xd3);//-set display offset
    lcdWriteCommand(0x00);//-not offset
    lcdWriteCommand(0xd5);//--set display clock divide ratio/oscillator frequency
    lcdWriteCommand(0xf0);//--set divide ratio
    lcdWriteCommand(0xd9);//--set pre-charge period
    lcdWriteCommand(0x22); //
    lcdWriteCommand(0xda);//--set com pins hardware configuration
    lcdWriteCommand(0x12);
    lcdWriteCommand(0xdb);//--set vcomh
    lcdWriteCommand(0x20);//0x20,0.77xVcc
    lcdWriteCommand(0x8d);//--set DC-DC enable
    lcdWriteCommand(0x14);//
    lcdWriteCommand(0xaf);//--turn on lcd panel 
}

void SSD1306::clearscreen(void)
{
    int16_t x, Y;
    for (Y=0;Y<LCD_Ymax/8;Y++) 
    {
        lcdSetAddr(0, Y);
      for (x=0;x<LCD_Xmax;x++)
         lcdWriteData(0x00);
    }
}

// print char function using Font5x7
void SSD1306::printC_5x7 (int x, int y, unsigned char ascii_code) 
{
    int8_t i;
    if (x<(LCD_Xmax-5) && y<(LCD_Ymax-7)) {
       if      (ascii_code<0x20) ascii_code=0x20;
     else if (ascii_code>0x7F) ascii_code=0x20;
       for (i=0;i<5;i++) {
              lcdSetAddr((x+i), (y/8)); 
              lcdWriteData(Font5x7[(ascii_code-0x20)*5+i]);
         }
    }
}

void SSD1306::printC(int Line, int Col, unsigned char ascii_code)
{
    uint8_t j, i, tmp;  
    for (j=0;j<2;j++) {     
        lcdSetAddr(Col*8, Line*2+j);
        for (i=0;i<8;i++) {
          tmp=Font8x16[(ascii_code-0x20)*16+j*8+i];
          lcdWriteData(tmp);
          }
  }
}

void SSD1306::printLine(int line, const char text[])
{
    uint8_t Col;
      for (Col=0; Col<strlen(text); Col++) 
            printC(line, Col, text[Col]);
}

void SSD1306::printS(int x, int y, const char text[])
{
    int8_t i;
    for (i=0;i<strlen(text);i++) 
        printC(x+i*8, y,text[i]);
}

void SSD1306::printS_5x7(int x, int y, const char text[])
{
    int8_t i;
    for (i=0;i<strlen(text);i++) {
        printC_5x7(x,y,text[i]);
      x=x+5;
    }
}

void SSD1306::drawPixel(int x, int y, int fgColor, int bgColor)
{
    if (fgColor!=0) 
        DisplayBuffer[x+y/8*LCD_Xmax] |= (0x01<<(y%8));
    else 
        DisplayBuffer[x+y/8*LCD_Xmax] &= (0xFE<<(y%8));

    lcdSetAddr(x, y/8);
    lcdWriteData(DisplayBuffer[x+y/8*LCD_Xmax]);
}

void SSD1306::drawBmp8x8(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,k, kx,ky;
    if (x<(LCD_Xmax-7) && y<(LCD_Ymax-7)) // boundary check     
         for (i=0;i<8;i++){
               kx=x+i;
                 t=bitmap[i];                    
                 for (k=0;k<8;k++) {
                          ky=y+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                }
         }
}

void SSD1306::drawBmp32x8(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,k, kx,ky;
    if (x<(LCD_Xmax-7) && y<(LCD_Ymax-7)) // boundary check     
         for (i=0;i<32;i++){
               kx=x+i;
                 t=bitmap[i];                    
                 for (k=0;k<8;k++) {
                          ky=y+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                }
         }
}

void SSD1306::drawBmp120x8(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,k, kx,ky;
    if (x<(LCD_Xmax-7) && y<(LCD_Ymax-7)) // boundary check     
         for (i=0;i<120;i++){
               kx=x+i;
                 t=bitmap[i];                    
                 for (k=0;k<8;k++) {
                          ky=y+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                }
         }
}

void SSD1306::drawBmp8x16(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,k, kx,ky;
    if (x<(LCD_Xmax-7) && y<(LCD_Ymax-7)) // boundary check     
         for (i=0;i<8;i++){
               kx=x+i;
                 t=bitmap[i];                    
                 for (k=0;k<8;k++) {
                          ky=y+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                }
                 t=bitmap[i+8];                  
                 for (k=0;k<8;k++) {
                          ky=y+k+8;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                }                
         }
}

void SSD1306::drawBmp16x8(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,k,kx,ky;
    if (x<(LCD_Xmax-15) && y<(LCD_Ymax-7)) // boundary check
         for (i=0;i<16;i++)
       {
               kx=x+i;
                 t=bitmap[i];                    
                 for (k=0;k<8;k++) {
                          ky=y+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                    }
         }
}

void SSD1306::drawBmp16x16(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,j,k, kx,ky;
    if (x<(LCD_Xmax-15) && y<(LCD_Ymax-15)) // boundary check
       for (j=0;j<2; j++){       
             for (i=0;i<16;i++) {   
            kx=x+i;
                      t=bitmap[i+j*16];                  
                      for (k=0;k<8;k++) {
                          ky=y+j*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }
             }
         }
}

void SSD1306::drawBmp16x24(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,j,k, kx,ky;
    if (x<(LCD_Xmax-15) && y<(LCD_Ymax-15)) // boundary check
       for (j=0;j<3; j++){       
             for (i=0;i<16;i++) {   
            kx=x+i;
                      t=bitmap[i+j*16];                  
                      for (k=0;k<8;k++) {
                          ky=y+j*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }
             }
         }
}

void SSD1306::drawBmp16x32(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t, i,j,k, kx,ky;
    if (x<(LCD_Xmax-15) && y<(LCD_Ymax-31)) // boundary check
       for (j=0;j<4; j++)   {            
             for (i=0;i<16;i++) {
            kx=x+i;
                      t=bitmap[i+j*16];                  
                      for (k=0;k<8;k++) {
                          ky=y+j*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }                    
             }       
         }
}

void SSD1306::drawBmp16x40(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t, i,j,k, kx,ky;
    if (x<(LCD_Xmax-15) && y<(LCD_Ymax-31)) // boundary check
       for (j=0;j<5; j++)   {            
             for (i=0;i<16;i++) {
            kx=x+i;
                      t=bitmap[i+j*16];                  
                      for (k=0;k<8;k++) {
                          ky=y+j*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }                   
             }       
         }
}

void SSD1306::drawBmp16x48(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,j,k,kx,ky;
    if (x<(LCD_Xmax-15) && y<(LCD_Ymax-47)) // boundary check
       for (j=0;j<6; j++)   {
         k=x;            
             for (i=0;i<16;i++) {
            kx=x+i;
                      t=bitmap[i+j*16];                  
                      for (k=0;k<8;k++) {
                          ky=y+j*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }                        
             }       
         }
}

void SSD1306::drawBmp16x64(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,j,k,kx,ky;
    if (x<(LCD_Xmax-15) && y==0) // boundary check
       for (j=0;j<8; j++) {
                 k=x;
             for (i=0;i<16;i++) {
            kx=x+i;
                      t=bitmap[i+j*16];                  
                      for (k=0;k<8;k++) {
                          ky=y+j*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }                        
             }
         }
}

void SSD1306::drawBmp32x16(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,jx,jy,k,kx,ky;
    if (x<(LCD_Xmax-31) && y<(LCD_Ymax-15)) // boundary check
        for (jy=0;jy<2;jy++)
       for (jx=0;jx<2;jx++) {
               k=x;
             for (i=0;i<16;i++) {
            kx=x+jx*16+i;
                      t=bitmap[i+jx*16+jy*32];                   
                      for (k=0;k<8;k++) {
                          ky=y+jy*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }                        
             }
            }
}

void SSD1306::drawBmp32x32(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,jx,jy,k, kx,ky;
    if (x<(LCD_Xmax-31) && y<(LCD_Ymax-31)) // boundary check
        for (jy=0;jy<4;jy++)
       for (jx=0;jx<2;jx++) {
               k=x;
             for (i=0;i<16;i++) {
            kx=x+jx*16+i;
                      t=bitmap[i+jx*16+jy*32];                   
                      for (k=0;k<8;k++) {
                          ky=y+jy*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }                        
             }
            }
}

void SSD1306::drawBmp32x48(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,jx,jy,k, kx,ky;
    if (x<(LCD_Xmax-31) && y<(LCD_Ymax-47)) // boundary check
        for (jy=0;jy<6;jy++)
       for (jx=0;jx<2;jx++) {
               k=x;
             for (i=0;i<16;i++) {
                      kx=x+jx*16+i;
                      t=bitmap[i+jx*16+jy*32];                   
                      for (k=0;k<8;k++) {
                          ky=y+jy*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }   
             }       
         }
}

void SSD1306::drawBmp32x64(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t,i,jx,jy,k, kx,ky;
    if (x<(LCD_Xmax-31) && y==0) // boundary check
        for (jy=0;jy<8;jy++)
       for (jx=0;jx<2;jx++) {
               k=x;
             for (i=0;i<16;i++) {
                      kx=x+jx*16+i;
                      t=bitmap[i+jx*16+jy*32];                   
                      for (k=0;k<8;k++) {
                          ky=y+jy*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }                        
             }
            }                
}

void SSD1306::drawBmp64x64(int x, int y, int fgColor, int bgColor, unsigned char bitmap[])
{
    uint8_t t, i,jx,jy,k, kx,ky;
    if (x<(LCD_Xmax-63) && y==0) // boundary check
        for (jy=0;jy<8;jy++)
       for (jx=0;jx<4;jx++) {
           k=x;
             for (i=0;i<16;i++) {
                      kx=x+jx*16+i;
                      t=bitmap[i+jx*16+jy*64];                 
                      for (k=0;k<8;k++) {
                          ky=y+jy*8+k;
                          if (t&(0x01<<k)) drawPixel(kx,ky,fgColor,bgColor);
                        }                    
             }
            }
}

void SSD1306::drawBMP(unsigned char *buffer)
{
  uint8_t x,y;
    for (x=0; x<LCD_Xmax; x++) {
        for (y=0; y<(LCD_Ymax/8); y++) {
               lcdSetAddr(x ,y);
               lcdWriteData(buffer[x+y*LCD_Xmax]);
            }
        }           
}

}   /* namespace mbed_nuvoton */
