#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <syslog.h>
#include <unistd.h>

#include <cmath> // required for pow()
#include <cstring>

using namespace std;

#include "CC1101.h"

// CC1101_XTAL: used to calculate frequencies, MUST match the quartz used
#ifndef CC1101_XTAL
#define CC1101_XTAL           26000000
#endif

// CC1101_TEST_COUNT: number of XTAL/192 tests for GDO0/2
#ifndef CC1101_TEST_COUNT
#define CC1101_TEST_COUNT     8192
#endif

// If GDO2 is connected check for CHIP_RDY this many times
#ifndef CC1101_CHIP_RDY_CHECKS
#define CC1101_CHIP_RDY_CHECKS  16
#endif

#ifdef CC1101_DEBUG_LOGGING
static const char *testedOK = "%s is tested OK";
#endif

CC1101::CC1101(SPISETUP &spi, vector<CC1101REG *> &setup)
	: spi(spi), chipSetup(setup) {
		if (wiringPiSPISetup(spi.channel, spi.speed) == -1) {
			throw runtime_error("can't open SPI channel");
		}
	}
void CC1101::waitForChipRdy(bool ignoreChipRdy) {
	// if GDO0 is connected then use it; anyway, it seems like CHIP_RDY goes
	// low only after chip reset and there's only a brief period of time when
	// it is low so to save wiring it can be replaced by the short wait
	if (ignoreChipRdy || spi.gdo0 == 0) {
		usleep(CC1101_CHIP_RDY_CHECKS * 10000);
		return;
	}
	for (int i = 0; i < CC1101_CHIP_RDY_CHECKS; i++) {
		if (digitalRead(spi.gdo0) == 0) {
			return;
		}
		usleep(10000);
	}
	throw runtime_error("CHIP_RDY didn't go low as it should");
}
int CC1101::saveReg(CC1101REG *reg) {
	uint8_t buffer[reg->SIZE + 1] { reg->CODE };
	if (reg->SIZE != 0) {
		buffer[1] = reg->getData();
	}
	if (wiringPiSPIDataRW(spi.channel, buffer, sizeof buffer) == -1) {
		snprintf(text, sizeof text, "wiringPi: returned error %d (%s)",
				errno, strerror(errno));
		throw runtime_error(text);
	}
	return buffer[reg->SIZE];
}
void CC1101::loadReg(CC1101REG *reg) {
	if (reg->SIZE == 0) {
		throw runtime_error("can't READ chip COMMAND");
	}
	uint8_t buffer[] { (unsigned char)(reg->CODE | 0x80), 0 };
	if (wiringPiSPIDataRW(spi.channel, buffer, sizeof buffer) == -1) {
		snprintf(text, sizeof text, "wiringPi: returned error %d (%s)",
				errno, strerror(errno));
		throw runtime_error(text);
	}
	reg->setData(buffer[1]);
}
int CC1101::resetChip(bool ignoreChipRdy) {
	int value;
	for (int i = 0; i < 5; i++) {
		SRES sres;
#ifdef CC1101_DEBUG_LOGGING
		syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), "Chip reset sent");
#endif
		if ((value = saveReg(&sres)) == 0x0f) {
			break;
		}
#ifdef CC1101_DEBUG_LOGGING
		syslog(LOG_MAKEPRI(LOG_USER, LOG_INFO), "Chip reset FAILED; retrying");
#endif
		usleep(CC1101_CHIP_RDY_CHECKS * 10000);
	}
	if (value != 0x0f) {
		throw runtime_error("SRES status byte must be 0x0f");
	}
#ifdef CC1101_DEBUG_LOGGING
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG),
			"Chip reset done; waiting for CHIP_RDY");
#endif
	waitForChipRdy(ignoreChipRdy);
	return value;
}
void CC1101::validateGDO0() {
	if (spi.gdo0 == 0) {
#ifdef CC1101_DEBUG_LOGGING
		syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), testedOK, "GDO0:Disabled");
#endif
		return;
	}

	IOCFG0 iocfg0;

	// test#1: XTAL/192 should give more or less equal number of samples
	saveReg(iocfg0.gdo0_cfg(0x3f));
	for (int i = 0, counter = 0; i < CC1101_TEST_COUNT; i++) {
		counter += (digitalRead(spi.gdo0) == 0 ? -1 : +1);
		if (abs(counter) > (CC1101_TEST_COUNT >> 4)) {
			throw runtime_error("GDO0 doesn't seem to be connected properly."
					" Did you run 'gpio export'?");
		}
	}

	// test#2: 3-STATE should only give lows (or '0's)
	saveReg(iocfg0.gdo0_cfg(0x2e)); // shutdown GDO0
	for (int i = 0; i < CC1101_TEST_COUNT; i++) {
		if (digitalRead(spi.gdo0) != 0) {
			throw runtime_error("GDO0 went 'high' in 3-state mode");
		}
	}
#ifdef CC1101_DEBUG_LOGGING
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), testedOK, "GDO0");
#endif
}
void CC1101::validateGDO2() {
	if (spi.gdo2 == 0) {
		throw runtime_error("GDO2 can't be disabled");
	}

	IOCFG2 iocfg2;

	// test#1: XTAL/192 should give more or less equal number of samples
	saveReg(iocfg2.gdo2_cfg(0x3f));
	for (int i = 0, counter = 0; i < CC1101_TEST_COUNT; i++) {
		counter += (digitalRead(spi.gdo2) == 0 ? -1 : +1);
		if (abs(counter) > (CC1101_TEST_COUNT >> 4)) {
			throw runtime_error("GDO2 doesn't seem to be connected properly."
					" Did you run 'gpio export'?");
		}
	}

	// test#2: 3-STATE should only give lows (or '0's)
	saveReg(iocfg2.gdo2_cfg(0x2e)); // shutdown GDO2
	for (int i = 0; i < CC1101_TEST_COUNT; i++) {
		if (digitalRead(spi.gdo2) != 0) {
			throw runtime_error("GDO2 went 'high' in 3-state mode");
		}
	}
#ifdef CC1101_DEBUG_LOGGING
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), testedOK, "GDO2");
#endif
}
void CC1101::checkWiring() {
	// checkWiring: validates CC1101 hardware presence and does some basic
	// functionality tests (ie. checks default register values etc)
#ifdef CC1101_DEBUG_LOGGING
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), "Using SPI: %s", spi.toString());
#endif

	resetChip(true);	// CHIP_RDY status must be ignored before the check
	// because we don't know if CHIP_RDY is connected
#ifdef CC1101_DEBUG_LOGGING
	// resetChip would've failed on error
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), testedOK, "SRES");
#endif

	IOCFG2 iocfg2;
	loadReg(&iocfg2);
	if (iocfg2.getData() != 0x29) {
		throw runtime_error("IOCFG2 should be 0x29 upon reset");
	}
#ifdef CC1101_DEBUG_LOGGING
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), testedOK, "IOCFG2");
#endif

	IOCFG1 iocfg1;
	loadReg(&iocfg1);
	if (iocfg1.getData() != 0x2e) {
		throw runtime_error("IOCFG1 should be 0x2e upon reset");
	}
#ifdef CC1101_DEBUG_LOGGING
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), testedOK, "IOCFG1");
#endif

	IOCFG0 iocfg0;
	loadReg(&iocfg0);
	if (iocfg0.getData() != 0x3f) {
		throw runtime_error("IOCFG0 should be 0x3f upon reset");
	}
#ifdef CC1101_DEBUG_LOGGING
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), testedOK, "IOCFG0");
#endif

	validateGDO2();
	validateGDO0();

#ifdef CC1101_DEBUG_LOGGING
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), testedOK, "CC1101");
#endif
}
const char *CC1101::getSetup() {
	FREQ0 freq0;
	loadReg(&freq0);

	FREQ1 freq1;
	loadReg(&freq1);

	FREQ2 freq2;
	loadReg(&freq2);

	double hz = (CC1101_XTAL / pow(2, 16)) * 
		(freq0.freq() | (freq1.freq() << 8) | (freq2.freq() << 16));

	FSCTRL1 fsctrl1;
	loadReg(&fsctrl1);
	double freqIf = (CC1101_XTAL / pow(2, 10)) * fsctrl1.freq_if();

	MDMCFG4 mdmcfg4;
	loadReg(&mdmcfg4);
	double bandwidth = CC1101_XTAL / (8 * (4 + mdmcfg4.chanbw_m())
			* pow(2, mdmcfg4.chanbw_e()));

	MDMCFG3 mdmcfg3;
	loadReg(&mdmcfg3);
	double drate = ((256 + mdmcfg3.drate_m()) * pow(2, mdmcfg4.drate_e())
			* CC1101_XTAL) / pow(2, 28);

	SNOPR snopr; // check current status and start listening again if needed
	int status = saveReg(&snopr);

	snprintf(text, sizeof text,
			"FR:%.2fMHz; IF:%.2fHz; BW:%.2fHz; DR:%.2fbaud; ST:%02x",
			(hz / 1e6), freqIf, bandwidth, drate, status);
	return text;
}
void CC1101::setup() {
	checkWiring(); // checkWiring leaves both GDO0&2 in 3-state mode
#ifdef CC1101_DEBUG_LOGGING
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG),
			"Writing PATABLE (even though it is unused for RX)");
#endif
	static uint8_t patable[] { // command + data (0 dBm; see page 60)
		0x7e, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};
	if (wiringPiSPIDataRW(spi.channel, patable, sizeof patable) == -1) {
		snprintf(text, sizeof text, "wiringPi: returned error %d (%s)",
				errno, strerror(errno));
		throw runtime_error(text);
	}
	for (CC1101REG * reg : chipSetup) {
#ifdef CC1101_DEBUG_LOGGING
		syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG),
				"setting reg %02x to %02x", reg->CODE, reg->getData());
#endif
		saveReg(reg);
	}
}
void CC1101::srx() {
#ifdef CC1101_DEBUG_LOGGING
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), "Switching to RX: %s", getSetup());
#endif
	SRX srx;
	saveReg(&srx);
}
CC1101DTA	*CC1101::getPacket(int index) {
	return &dataCollected[index];
}
void CC1101::classCallback() {
#ifdef CC1101_DEBUG_LOGGING
	// semaphore is locked so we're free to use SPI for getSetup
	syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), "classCallback: %s", getSetup());
#endif

	RXBYTES rxbytes; // check how many bytes are in FIFO
	loadReg(&rxbytes);

	if (rxbytes.rxfifo_overflow() != 0) {
		syslog(LOG_MAKEPRI(LOG_USER, LOG_WARNING),
				"C1101 FIFO overflow! Resetting with SFRX.");
		SFRX sfrx;
		saveReg(&sfrx);
	} else {
		if (rxbytes.num_rxbytes() != 0) {
			dataCollected[dataCollectedIdx].data[0] = 0xff; // FIFO BATCH READ cmd
			if (wiringPiSPIDataRW(spi.channel, dataCollected[dataCollectedIdx].data,
						rxbytes.num_rxbytes() + 1) == -1) { // Command + FIFO length
				throw runtime_error("SPI error reading RX FIFO");
			}
			dataCollected[dataCollectedIdx].data[0] = rxbytes.num_rxbytes(); // len
			if (dataCollected[dataCollectedIdx].decode() != 0) { // is timestamp set?
				dataCollectedIdx = (dataCollectedIdx + 1) % CC1101_NBUFFERS;
			}
		}
	}
	SNOPR snopr; // check current status and start listening again if needed
	if ((saveReg(&snopr) & 0x70) != 0x10) {
		srx();
	}
}
