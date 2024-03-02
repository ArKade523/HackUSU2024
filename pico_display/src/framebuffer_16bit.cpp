#include "framebuffer_16bit.h"

FrameBuffer16Bit::FrameBuffer16Bit(uint16_t width, uint16_t height)
    : width(width), height(height), size(width * height * sizeof(uint16_t)) {
  buffer = new uint8_t[size];
  memset(buffer, 0, size);
}

void FrameBuffer16Bit::fill(uint16_t color) {
  for (int i = 0; i < width * height; i++) {
    buffer[i * 2] = static_cast<uint8_t>(color >> 8);
    buffer[i * 2 + 1] = static_cast<uint8_t>(color & 0xFF);
  }
}

void FrameBuffer16Bit::setPixel(uint16_t x, uint16_t y, uint16_t color) {
  if (x < width && y < height) {
    buffer[(y * width + x) * 2] = static_cast<uint8_t>(color >> 8);
    buffer[(y * width + x) * 2 + 1] = static_cast<uint8_t>(color & 0xFF);
  }
}

void FrameBuffer16Bit::drawLine(uint16_t x0, uint16_t y0, uint16_t x1,
                                uint16_t y1, uint16_t color) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  for (;;) {
    setPixel(x0, y0, color);
    if (x0 == x1 && y0 == y1)
      break;
    e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void FrameBuffer16Bit::drawCircle(uint16_t x0, uint16_t y0, uint16_t radius,
                                  uint16_t color) {
  int x = radius, y = 0;
  int radiusError = 1 - x;

  while (x >= y) {
    setPixel(x + x0, y + y0, color);
    setPixel(y + x0, x + y0, color);
    setPixel(-x + x0, y + y0, color);
    setPixel(-y + x0, x + y0, color);
    setPixel(-x + x0, -y + y0, color);
    setPixel(-y + x0, -x + y0, color);
    setPixel(x + x0, -y + y0, color);
    setPixel(y + x0, -x + y0, color);
    y++;
    if (radiusError < 0) {
      radiusError += 2 * y + 1;
    } else {
      x--;
      radiusError += 2 * (y - x + 1);
    }
  }
}

void FrameBuffer16Bit::drawCircleFill(uint16_t x0, uint16_t y0, uint16_t radius,
                                      uint16_t color) {
  int x = radius, y = 0;
  int radiusError = 1 - x;

  while (x >= y) {
    drawLine(-x + x0, y + y0, x + x0, y + y0, color);
    drawLine(-y + x0, x + y0, y + x0, x + y0, color);
    drawLine(-x + x0, -y + y0, x + x0, -y + y0, color);
    drawLine(-y + x0, -x + y0, y + x0, -x + y0, color);
    y++;
    if (radiusError < 0) {
      radiusError += 2 * y + 1;
    } else {
      x--;
      radiusError += 2 * (y - x + 1);
    }
  }
}

void FrameBuffer16Bit::drawRect(uint16_t x, uint16_t y, uint16_t width,
                                uint16_t height, uint16_t color) {
  drawLine(x, y, x + width, y, color);
  drawLine(x, y, x, y + height, color);
  drawLine(x + width, y, x + width, y + height, color);
  drawLine(x, y + height, x + width, y + height, color);
}

void FrameBuffer16Bit::drawRectFill(uint16_t x, uint16_t y, uint16_t width,
                                    uint16_t height, uint16_t color) {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      setPixel(x + i, y + j, color);
    }
  }
}

void FrameBuffer16Bit::drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1,
                                    uint16_t y1, uint16_t x2, uint16_t y2,
                                    uint16_t color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

void FrameBuffer16Bit::drawTriangleFill(uint16_t x0, uint16_t y0, uint16_t x1,
                                        uint16_t y1, uint16_t x2, uint16_t y2,
                                        uint16_t color) {
  auto swap = [](auto &a, auto &b) {
    auto temp = a;
    a = b;
    b = temp;
  };

  // Sort vertices by y-coordinate
  if (y1 < y0) {
    swap(y0, y1);
    swap(x0, x1);
  }
  if (y2 < y0) {
    swap(y0, y2);
    swap(x0, x2);
  }
  if (y2 < y1) {
    swap(y1, y2);
    swap(x1, x2);
  }

  int16_t dx1 = x1 - x0, dy1 = y1 - y0;
  int16_t dx2 = x2 - x0, dy2 = y2 - y0;
  int16_t dx3 = x2 - x1, dy3 = y2 - y1;

  int16_t sa = 0, sb = 0;
  for (int16_t y = y0; y <= y1; ++y) {
    int16_t a = x0 + sa / dy1;
    int16_t b = x0 + sb / dy2;
    sa += dx1;
    sb += dx2;
    if (a > b)
      swap(a, b);
    drawLine(a, y, b, y, color);
  }

  sa = dx3 * (y1 - y1);
  sb = dx2 * (y1 - y0);
  for (int16_t y = y1; y <= y2; ++y) {
    int16_t a = x1 + sa / dy3;
    int16_t b = x0 + sb / dy2;
    sa += dx3;
    sb += dx2;
    if (a > b)
      swap(a, b);
    drawLine(a, y, b, y, color);
  }
}

void FrameBuffer16Bit::blit(FrameBuffer16Bit &src, uint16_t x, uint16_t y) {
  for (int i = 0; i < src.getWidth(); i++) {
    for (int j = 0; j < src.getHeight(); j++) {
      uint16_t color = src.getPixel(i, j);
      setPixel(i + x, j + y, color);
    }
  }
}

void FrameBuffer16Bit::text(const std::string &str, uint16_t x, uint16_t y,
                            uint16_t color) {
  for (int i = 0; i < str.length(); i++) {
    char c = str[i];
    for (int j = 0; j < 8; j++) {
      uint8_t line = font8x8[c][j];
      for (int k = 0; k < 8; k++) {
        if (line & (1 << k)) {
          setPixel(x + i * 8 + k, y + j, color);
        }
      }
    }
  }
}

void FrameBuffer16Bit::clear() { memset(buffer, 0, size); }