#ifndef PICO_DISPLAY_SPI_H
#define PICO_DISPLAY_SPI_H

#include "framebuffer_16bit.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <cstdint>

class DisplaySPI {
public:
  DisplaySPI(uint16_t width, uint16_t height, spi_inst_t *spi, uint8_t cs,
             uint8_t dc, uint8_t sck, uint8_t mosi) : 
             width(width), height(height), spi(spi), cs(cs), dc(dc), sck(sck), mosi(mosi) {};
  virtual void initDisplay() = 0;
  virtual void powerOn() = 0;
  virtual void reset() = 0;
  virtual void fill(uint16_t color) = 0;
  virtual void setPixel(uint16_t x, uint16_t y, uint16_t color) = 0;
  virtual void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                        uint16_t color) = 0;
  virtual void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius,
                          uint16_t color) = 0;
  virtual void drawCircleFill(uint16_t x0, uint16_t y0, uint16_t radius,
                              uint16_t color) = 0;
  virtual void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                        uint16_t color) = 0;
  virtual void drawRectFill(uint16_t x, uint16_t y, uint16_t width,
                            uint16_t height, uint16_t color) = 0;
  virtual void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                            uint16_t x2, uint16_t y2, uint16_t color) = 0;
  virtual void drawTriangleFill(uint16_t x0, uint16_t y0, uint16_t x1,
                                uint16_t y1, uint16_t x2, uint16_t y2,
                                uint16_t color) = 0;
  virtual void blit(FrameBuffer16Bit &src, uint16_t x, uint16_t y) = 0;
  virtual void text(const std::string &str, uint16_t x, uint16_t y, uint16_t color) = 0;
  virtual void show() = 0;

protected:
  void initSPI();
  void initDrawing();
  void writeCmd(uint8_t cmd);
  void writeData(const uint8_t* data, size_t len);

  uint16_t width, height;
  spi_inst_t *spi;
  uint8_t cs, dc, sck, mosi;
};

#endif // PICO_DISPLAY_SPI_H