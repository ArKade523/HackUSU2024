#include "ili9341.h"

#define SPI_PORT spi0

int main(void) {
  stdio_init_all();

  printf("Waiting for input...\n");

  // Wait for a character to be received over UART
  // getchar();

  printf("Starting...\n");

  ILI9341 display(240, 320, SPI_PORT, 13, 15, 6, 7);

  printf("Display created\n");

  display.initSPI();

  printf("SPI initialized\n");

  display.initDisplay();

  printf("Display initialized\n");

  display.fill(0);

  display.show();

  uint16_t x = 120, y = 160, radius = 20;
  int16_t dx = 4, dy = 4;
  uint8_t r = 31, g = 0, b = 0; // Start with red
  while (1) {
    display.fill(0);
    uint16_t color = (r << 11) | (g << 5) | b; // Convert RGB to RGB565
    display.drawCircleFill(x, y, radius, color);
    display.show();
    x += dx;
    y += dy;
    if (x + radius >= 240 || x - radius <= 0) {
      dx = -dx;
    }
    if (y + radius >= 320 || y - radius <= 0) {
      dy = -dy;
    }

    // Cycle through colors
    if (r > 0 && b == 0) {
      r--;
      g++;
    } else if (g > 0 && r == 0) {
      g--;
      b++;
    } else if (b > 0 && g == 0) {
      b--;
      r++;
    }

    sleep_ms(10);
  }

  return 0;
}