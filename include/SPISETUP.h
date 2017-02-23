#ifndef __SPISETUP_H__
#define __SPISETUP_H__
#include <string>

#include "config.h"

struct SPISETUP {
  int channel = 0;
  int speed = 750000;
  int gdo0 = 0; // don't test, use as analog thermometer
  int gdo2 = 24; // mode 0x06: mark the packet reception
  char text[TEXT_BUFFER_LENGTH];

  virtual char *toString() { // Java-style
    snprintf(text, sizeof text,
      "SPI: channel:%d; speed:%d; gdo0:%d; gdo2:%d", channel, speed, gdo0, gdo2);
    return text;
  }
};
#endif // __SPISETUP_H__
