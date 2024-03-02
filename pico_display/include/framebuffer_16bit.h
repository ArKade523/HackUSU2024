#ifndef FRAMEBUFFER_16BIT_H
#define FRAMEBUFFER_16BIT_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include "font8x8.h"

class FrameBuffer16Bit {
public:
    FrameBuffer16Bit(uint16_t width, uint16_t height);
    void fill(uint16_t color);
    void setPixel(uint16_t x, uint16_t y, uint16_t color);
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);
    void drawCircleFill(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);
    void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    void drawRectFill(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
    void drawTriangleFill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
    void blit(FrameBuffer16Bit &src, uint16_t x, uint16_t y);
    void text(const std::string &str, uint16_t x, uint16_t y, uint16_t color);
    void clear();

    uint8_t *getBuffer() { return buffer; }
    uint16_t getWidth() { return width; }
    uint16_t getHeight() { return height; }
    size_t getSize() { return size; }

    uint16_t getPixel(uint16_t x, uint16_t y) {
        return static_cast<uint16_t>(buffer[(y * width + x) * 2] << 8) | buffer[(y * width + x) * 2 + 1];
    }

protected:
    uint8_t *buffer;
    uint16_t width, height;
    size_t size;
};

#endif // FRAMEBUFFER_16BIT_H