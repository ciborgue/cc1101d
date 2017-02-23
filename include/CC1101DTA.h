#ifndef __CC1101DTA_H__
#define __CC1101DTA_H__
#include <time.h>
#include <stdint.h>

#include <cstdio>

struct CC1101DTA {
  time_t	tmstamp = 0; // when this packet was decoded
	bool		syslogged = false;
  uint8_t	data[68]; // len + FIFO size + LQI + RSSI + 0
  char		text[256]; // toString buffer

  uint8_t	chkSum(); // calculate actual chkSum
  uint8_t	crc8() { return 0; }; // TODO calculate actual crc8
  time_t	decode(); // decode bytes -> nibbles

  uint32_t	getRCID(); // sensorId + channel + rollingCode

  // Accessors for the 1d20 message format
  uint16_t	getSensorId();
  uint8_t	getChannel();
  uint8_t	getRollingCode();
  uint8_t	getFlags();
  double	getTemperature();
  double	getHumidity();
  uint8_t	getUnknown1d20();
  uint8_t	getChkSum(); // get decoded chkSum
  uint8_t	getPostAmble();
  double	getLQI();
  double	getRSSI();
  virtual	const char *toString();
  virtual	const char *toJSON();
};
#endif // __CC1101DTA_H__
