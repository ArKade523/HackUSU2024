#include "ili9341.h"

#define SPI_PORT spi0
#define PIN_MISO 14
#define PIN_CS 13
#define PIN_SCK 6
#define PIN_MOSI 7
#define PIN_DC 15
#define PIN_RST 14
#define PIN_LED 25

ILI9341::ILI9341(uint16_t width, uint16_t height, spi_inst_t *spi, uint8_t cs,
                 uint8_t dc, uint8_t sck, uint8_t mosi)
    : DisplaySPI(width, height, spi, cs, dc, sck, mosi),
      frameBuffer(width, height) {}

void ILI9341::initDisplay() {
  printf("Initializing display\n");
  gpio_put(PIN_CS, 0); // CS low (selected)
  printf("CS low\n");
  reset();
  printf("Reset\n");

  // Initialization sequence
  static const struct {
    uint8_t cmd;
    uint8_t data[15]; // Adjust size to fit the largest data set
    size_t len;
  } init_cmds[] = {
      {0xEF, {0x03, 0x80, 0x02}, 3}, // Undocumented command, literally no idea
      {ILI9341_CMD_POWER_CONTROL_B, {0x00,
         0xC1, // ?? Should be 0x81, 0x85, 0x91, or 0x95
         0x30  // DC_en = 0
        }, 3},
      {ILI9341_CMD_POWER_ON_SEQUENCE_CONTROL, 
        {0x64, // CP1 soft start keep frame 1, CP 23 soft start keep frame 3
         0x03, // En_vcl 1st frame, En_ddvdh 4th frame
         0x12, // En_vgh 2nd frame, En_vgl 3rd frame
         0x81  // DDVDH enhance mode on
        }, 4},
      {ILI9341_CMD_DRIVER_TIMING_CONTROL_A, 
        {0x85, // default + 1 unit overlap timing control
         0x00, // default 1 unit EQ timing control, default 1 unit CR timing control
         0x78  // deafult 2 unit pre-charge timing control
        }, 3},
      {ILI9341_CMD_POWER_CONTROL_A, {0x39,0x2C, 0x00, 
         0x34, // vcore control 1.6V
         0x02  // ddvh control 5.6V
        }, 5},
      {ILI9341_CMD_PUMP_RATIO_CONTROL, 
        {0x20  // DDVDH=2xVCI
        }, 1},
      {ILI9341_CMD_DRIVER_TIMING_CONTROL_B, 
        {0x00, // 0 unit gate line timing control
         0x00}, 2},
      {ILI9341_CMD_POWER_CONTROL_1, 
        {0x23  // VRH[5:0] = 4.6V
        }, 1},
      {ILI9341_CMD_POWER_CONTROL_2, 
        {0x10  // DDVH = VCIx2, VGH = VCIx7, VGL = VCIx4
        }, 1},
      {ILI9341_CMD_VCOM_CONTROL_1, 
        {0x3E, // VCOMH = 4.250V
         0x28  // VCOML = -1.500V
        }, 2},
      {ILI9341_CMD_VCOM_CONTROL_2, 
        {0x86  // VMH - 54 VML - 54
        }, 1},
      {ILI9341_CMD_MEMORY_ACCESS_CONTROL, 
        {0x48  // MY = 0, MX = 1, MV = 0, ML = 0, BGR = 1, MH = 0
        }, 1},
      {ILI9341_CMD_PIXEL_FORMAT_SET, 
        {0x55  // 16-bit RGB
        }, 1},
      {ILI9341_CMD_FRAME_CONTROL_NORMAL_MODE,
        {0x00, // Division ratio = fosc, no division
         ILI9341_FRAMERATE_119_HZ
        }, 2},
      {ILI9341_CMD_DISPLAY_FUNCTION_CONTROL, 
        {0x08, // Interval scan control = 1, no display shift
         0x82, // REV = 1, GS = 0, SS = 0, SM = 0, ISC = 2
         0x27  // NL = 320 lines (320x240 display)
        }, 3},
      {ILI9341_CMD_ENABLE_3G, 
        {0x02  // Disable 3 gamma control
        }, 1},
      {ILI9341_CMD_GAMMA_SET, 
        {0x01  // Gamma curve 1 (G2.2)
        }, 1},
      {ILI9341_CMD_POSITIVE_GAMMA_CORRECTION,
        {0x0F, // VP63 = 0x0F
         0x31, // VP62 = 0x31
         0x2B, // VP61 = 0x2B
         0x0C, // VP59 = 0x0C
         0x0E, // VP57 = 0x0E
         0x08, // VP50 = 0x08
         0x4E, // VP43 = 0x4E
         0xF1, // VP27 = 0xF, VP36 = 0x1
         0x37, // VP20 = 0x37
         0x07, // VP13 = 0x07
         0x10, // VP6 = 0x10
         0x03, // VP4 = 0x03
         0x0E, // VP2 = 0x0E
         0x09, // VP1 = 0x09
         0x00  // VP0 = 0x00
        }, 15},
      {ILI9341_CMD_NEGATIVE_GAMMA_CORRECTION,
        {0x00, // VN63 = 0x00
         0x0E, // VN62 = 0x0E
         0x14, // VN61 = 0x14
         0x03, // VN59 = 0x03
         0x11, // VN57 = 0x11
         0x07, // VN50 = 0x07
         0x31, // VN43 = 0x31
         0xC1, // VN36 = 0xC, VN27 = 0x1
         0x48, // VN20 = 0x48
         0x08, // VN13 = 0x08
         0x0F, // VN6 = 0x0F
         0x0C, // VN4 = 0x0C
         0x31, // VN2 = 0x31
         0x36, // VN1 = 0x36
         0x0F  // VN0 = 0x0F
        }, 15},
      {ILI9341_CMD_SLEEP_OUT, {0}, 0},
      {ILI9341_CMD_DISPLAY_ON, {0}, 0},
      {ILI9341_CMD_NORMAL_DISPLAY_MODE_ON, {0}, 0},
  };

  printf("Sending commands\n");
  for (size_t i = 0; i < sizeof(init_cmds) / sizeof(init_cmds[0]); ++i) {
    writeCmd(init_cmds[i].cmd);
    printf("Command %d\n", i);
    if (init_cmds[i].len > 0) {
      writeData(init_cmds[i].data, init_cmds[i].len);
    }
    if (init_cmds[i].cmd == ILI9341_CMD_SLEEP_OUT ||
        init_cmds[i].cmd == ILI9341_CMD_DISPLAY_ON) {
      sleep_ms(120); // Wait after sleep out and display on commands
    }
  }

  printf("Sent all commands\n");

  gpio_put(PIN_CS, 1); // CS high (deselected)
  printf("CS high\n");
}

void ILI9341::initSPI() {
  printf("Initializing SPI\n");
  spi_init(SPI_PORT, 1000000000);
  printf("SPI initialized\n");
  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);

  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

  gpio_init(PIN_CS);
  gpio_init(PIN_DC);
  gpio_init(PIN_RST);

  gpio_set_dir(PIN_CS, GPIO_OUT);
  gpio_set_dir(PIN_DC, GPIO_OUT);
  gpio_set_dir(PIN_RST, GPIO_OUT);
  gpio_put(PIN_CS, 1); 
  gpio_put(PIN_DC, 0);   
  gpio_put(PIN_RST, 0);
}

void ILI9341::powerOn() {
  writeCmd(ILI9341_CMD_DISPLAY_ON);
  sleep_ms(120);
}

void ILI9341::reset() {
  gpio_put(PIN_RST, 0);
  sleep_ms(50);
  gpio_put(PIN_RST, 1);
  sleep_ms(50);
}

void ILI9341::fill(uint16_t color) {
  frameBuffer.fill(color);
}

void ILI9341::setPixel(uint16_t x, uint16_t y, uint16_t color) {
  frameBuffer.setPixel(x, y, color);
}

void ILI9341::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                       uint16_t color) {
  frameBuffer.drawLine(x0, y0, x1, y1, color);
}

void ILI9341::drawCircle(uint16_t x0, uint16_t y0, uint16_t radius,
                         uint16_t color) {
  frameBuffer.drawCircle(x0, y0, radius, color);
}

void ILI9341::drawCircleFill(uint16_t x0, uint16_t y0, uint16_t radius,
                             uint16_t color) {
  frameBuffer.drawCircleFill(x0, y0, radius, color);
}

void ILI9341::drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                       uint16_t color) {
  frameBuffer.drawRect(x, y, width, height, color);
}

void ILI9341::drawRectFill(uint16_t x, uint16_t y, uint16_t width,
                           uint16_t height, uint16_t color) {
  frameBuffer.drawRectFill(x, y, width, height, color);
}

void ILI9341::drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                           uint16_t x2, uint16_t y2, uint16_t color) {
  frameBuffer.drawTriangle(x0, y0, x1, y1, x2, y2, color);
}

void ILI9341::drawTriangleFill(uint16_t x0, uint16_t y0, uint16_t x1,
                               uint16_t y1, uint16_t x2, uint16_t y2,
                               uint16_t color) {
  frameBuffer.drawTriangleFill(x0, y0, x1, y1, x2, y2, color);
}

void ILI9341::blit(FrameBuffer16Bit &src, uint16_t x, uint16_t y) {
  frameBuffer.blit(src, x, y);
}

void ILI9341::text(const std::string &str, uint16_t x, uint16_t y, uint16_t color) {
  frameBuffer.text(str, x, y, color);
}

void ILI9341::show() {
  writeCmd(ILI9341_CMD_COLUMN_ADDRESS_SET);
  uint8_t columnStart[] = {0x00, 0x00};
  writeData(columnStart, 2); // Column start

  uint8_t columnEnd[] = {0x00, 0xEF};
  writeData(columnEnd, 2); // Column end

  writeCmd(ILI9341_CMD_PAGE_ADDRESS_SET);

  uint8_t pageStart[] = {0x00, 0x00};
  writeData(pageStart, 2); // Page start

  uint8_t pageEnd[] = {0x01, 0x3F};
  writeData(pageEnd, 2); // Page end

  writeCmd(ILI9341_CMD_MEMORY_WRITE);
  writeData(frameBuffer.getBuffer(), frameBuffer.getSize());
  writeCmd(ILI9341_CMD_NO_OPERATION);
}

void ILI9341::writeCmd(uint8_t cmd) {
  printf("Writing command %d\n", cmd);
  gpio_put(PIN_DC, 0); // DC low (command)
  printf("DC low\n");
  gpio_put(PIN_CS, 0); // CS low (selected)
  printf("CS low\n");
  spi_write_blocking(SPI_PORT, &cmd, 1);
  printf("Command written\n");
  gpio_put(PIN_CS, 1); // CS high (deselected)
  printf("Command written\n");
}

void ILI9341::writeData(const uint8_t *data, size_t len) {
  gpio_put(PIN_DC, 1); // DC high (data)
  gpio_put(PIN_CS, 0); // CS low (selected)
  spi_write_blocking(SPI_PORT, data, len);
  gpio_put(PIN_CS, 1); // CS high (deselected)
}