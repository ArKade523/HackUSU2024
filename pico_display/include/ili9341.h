#ifndef PICO_ILI9341_H
#define PICO_ILI9341_H

#include "display_spi.h"
#include "ili9341_cmd.h"

// ILI9341 displays are able to update at any rate between 61Hz to up to 119Hz. Default at power on is 70Hz.
#define ILI9341_FRAMERATE_61_HZ 0x1F
#define ILI9341_FRAMERATE_63_HZ 0x1E
#define ILI9341_FRAMERATE_65_HZ 0x1D
#define ILI9341_FRAMERATE_68_HZ 0x1C
#define ILI9341_FRAMERATE_70_HZ 0x1B
#define ILI9341_FRAMERATE_73_HZ 0x1A
#define ILI9341_FRAMERATE_76_HZ 0x19
#define ILI9341_FRAMERATE_79_HZ 0x18
#define ILI9341_FRAMERATE_83_HZ 0x17
#define ILI9341_FRAMERATE_86_HZ 0x16
#define ILI9341_FRAMERATE_90_HZ 0x15
#define ILI9341_FRAMERATE_95_HZ 0x14
#define ILI9341_FRAMERATE_100_HZ 0x13
#define ILI9341_FRAMERATE_106_HZ 0x12
#define ILI9341_FRAMERATE_112_HZ 0x11
#define ILI9341_FRAMERATE_119_HZ 0x10


class ILI9341 : public DisplaySPI {
public:
  ILI9341(uint16_t width, uint16_t height, spi_inst_t *spi, uint8_t cs,
          uint8_t dc, uint8_t sck, uint8_t mosi);
  void initDisplay();
  void initSPI();
  void powerOn();
  void reset();
  void fill(uint16_t color);
  void setPixel(uint16_t x, uint16_t y, uint16_t color);
  void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                uint16_t color);
  void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);
  void drawCircleFill(uint16_t x0, uint16_t y0, uint16_t radius,
                      uint16_t color);
  void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                uint16_t color);
  void drawRectFill(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                    uint16_t color);
  void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                    uint16_t x2, uint16_t y2, uint16_t color);
  void drawTriangleFill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                        uint16_t x2, uint16_t y2, uint16_t color);
  void blit(FrameBuffer16Bit &src, uint16_t x, uint16_t y);
  void text(const std::string &str, uint16_t x, uint16_t y, uint16_t color);
  void show();

protected:
    void writeCmd(uint8_t cmd);
    void writeData(const uint8_t* data, size_t len);

    uint16_t width, height;
    spi_inst_t *spi;
    uint8_t cs, dc, sck, mosi;
    FrameBuffer16Bit frameBuffer;
    
};

#endif // PICO_ILI9341_H