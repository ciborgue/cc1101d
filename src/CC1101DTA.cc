#include <string>
#include <cstring>
#include <syslog.h>

#include "CC1101DTA.h"

uint8_t CC1101DTA::chkSum() {
  uint8_t value = 0x00;
  for (int i = 1; i < 16; i++) {
    value += data[i];
  }
  return value;
}
time_t CC1101DTA::decode() {
	// returns 0 if data can't be decoded; UNIX timestamp on success.
	const static char XDIGIT[] = "0123456789abcdef";
	const static uint8_t XLAT[] {
		0B10101010, 0B01101010, 0B10011010, 0B01011010, 
		0B10100110, 0B01100110, 0B10010110, 0B01010110, 
		0B10101001, 0B01101001, 0B10011001, 0B01011001, 
		0B10100101, 0B01100101, 0B10010101, 0B01010101,
	};

	// make a temporary separate buffer for the packet hexdump
	// this part can be taken away as the decode procedure
	// modifies the original data
	uint8_t packetDump[65 * 3]; // LEN + FIFO is 64 bytes
	for (int sIdx = 0; sIdx < data[0] + 1; sIdx++) {
		packetDump[sIdx * 3 + 0] = XDIGIT[(data[sIdx] >> 4) & 0x0f];
		packetDump[sIdx * 3 + 1] = XDIGIT[data[sIdx] & 0x0f];
		packetDump[sIdx * 3 + 2] = ' ';
	}
	packetDump[2] = ':'; // just cosmetics
	packetDump[data[0] * 3 + 2] = '\0';

	tmstamp = 0;
	// data[0] contains packet length + 2 (LQI and RSSI) thus we're trying
	// to decode data from data[1] minus last TWO bytes
	for (int sIdx = 1; sIdx < data[0] - 1; sIdx++) {
		int nibble = 0x10; // 0x10 is invalid as nibble is 0..f
		for (int idx = 0; idx < 0x10; idx++) {
			if (XLAT[idx] == data[sIdx]) {
				nibble = idx; // decode a nibble 8bit -> 4bit
				break;
			}
		}
		if ((nibble & 0xf0) != 0) {
			syslog(LOG_MAKEPRI(LOG_DAEMON, LOG_WARNING),
					"Package can't be decoded (bad nibble): %s", packetDump);
			return tmstamp; // nibble can't be decoded; packet is invalid
		}
		data[sIdx] = nibble;
	}
	if (chkSum() != getChkSum()) {
		syslog(LOG_MAKEPRI(LOG_DAEMON, LOG_WARNING),
				"Package can't be decoded (bad chksum): %s", packetDump);
		return tmstamp; // chkSum doesn't match; packet is invalid
	}

#ifdef CC1101_DEBUG_LOGGING
	for (int sIdx = 0; sIdx < data[0] + 1; sIdx++) {
		packetDump[sIdx * 3 + 0] = XDIGIT[(data[sIdx] >> 4) & 0x0f];
		packetDump[sIdx * 3 + 1] = XDIGIT[data[sIdx] & 0x0f];
		packetDump[sIdx * 3 + 2] = ' ';
	}
	packetDump[2] = ':'; // just cosmetics
	packetDump[data[0] * 3 + 2] = '\0';

	syslog(LOG_MAKEPRI(LOG_DAEMON, LOG_DEBUG),
			"packet: %s", packetDump);
#endif

	return (tmstamp = time(NULL));
}
uint32_t CC1101DTA::getRCID() {
  return (getSensorId() << 16) | (getChannel() << 8) | getRollingCode();
}
uint16_t CC1101DTA::getSensorId() {
  uint16_t hi = (data[1] << 4) | data[2];
  uint16_t lo = (data[3] << 4) | data[4];
  return (hi << 8) | lo;
}
uint8_t CC1101DTA::getChannel() {
  return (data[5] == 4 ? 3 : data[5]);
}
uint8_t CC1101DTA::getRollingCode() {
  return (data[6] << 4) | data[7];
}
uint8_t CC1101DTA::getFlags() {
  return data[8];
}
double CC1101DTA::getTemperature() {
  return (data[12] == 0 ? 1 : -1)
    * (data[11] * 10.0 + data[10] + data[9] * 0.1);
}
double CC1101DTA::getHumidity() {
  return data[14] * 10.0 + data[13];
}
uint8_t CC1101DTA::getUnknown1d20() {
  return data[15];
}
uint8_t CC1101DTA::getChkSum() {
  return (data[17] << 4) | data[16];
}
uint8_t CC1101DTA::getPostAmble() {
  return (data[19] << 4) | data[18];
}
double CC1101DTA::getLQI() {
  return data[20]; // TODO; process LQI
}
double CC1101DTA::getRSSI() {
  int RSSI_offset = 74; // world constant; see page 44
  if (data[21] < 128) {
    return (data[21] / 2.0) - RSSI_offset;
  } else {
    return ((data[21] - 256) / 2.0) - RSSI_offset;
  }
}
const char *CC1101DTA::toString() {
  strncpy(text, "tm:", sizeof text);

  int out = strlen(text);
  strftime(text + out, sizeof text - out,
      "\"%Y-%m-%d %T %z\"", localtime(&tmstamp));

  out = strlen(text);
  snprintf(text + out, sizeof text - out,
      ", id:%04X, ch:%02d; rc:%02X"
      ", t:%+5.1f°c, h:%2.0f%%, lqi:%3.0f, rssi:%+5.1fdBm"
			", fl:%x, unk:%x",
      getSensorId(), getChannel(), getRollingCode(),
      getTemperature(), getHumidity(), getLQI(), getRSSI(),
			getFlags(), getUnknown1d20()
	);
  return text;
}
const char *CC1101DTA::toJSON() {
  snprintf(text, sizeof text,
      "\"CC1101%04X%02X%02X\": {\"timestamp\": ",
      getSensorId(), getChannel(), getRollingCode());

  int out = strlen(text);
  strftime(text + out, sizeof text - out,
      "\"%Y-%m-%d %T %z\"", localtime(&tmstamp));

  out = strlen(text);
  snprintf(text + out, sizeof text - out,
      ", \"temperature\": %.1f, \"humidity\": %.0f}, ",
      getTemperature(), getHumidity());
  return text;
}
