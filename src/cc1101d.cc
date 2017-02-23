#include <wiringPi.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#include <syslog.h>
#include <unistd.h>
#include <getopt.h>

#include <ctime>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

#include "config.h" // autogenerated; used for cosmetics mostly
#include "CC1101.h"

#include "SysSem.h"

// Even though it is technically possible to have TWO receivers per
// RPi (using CS0 and CS1) in reality that does not make much sense
// this'd require different settings for the radio (assuming different
// frequency/baudrate/modulation packet format should you decide to
// support some other weather sensor than Oregon Scientific #1D20).
// Even for a different model of the Oregon Scientific sensor with
// the same 2.1 protocol the decoding procedure is going to be different.
// Quite frankly I'm not so sure about using Oregon Scientific receivers
// anymore but all needed research is already done and they'll stay.
//
// As a compromise, I'm keeping support for the single CC1101 receiver
// collecting data from Oregon Scientific 1D20 transmitters with some
// room for improvement but I'm not interested in supporting the second
// radio module or any different sensor model. After all this project
// is all about marijuana grow and all I need is a temperature (humidity
// is just a bonus; I can't control it and I can't care less).
//
// Adding another v2.1 Oregon Scientific sensor is easy:
//  1. Make sure packet is going to fit (radio setup->PKTLEN); right now
//      it fits 20 bytes (len+data) for 1D20. This is enough for
//      thermometer/hygrometer but might not be enough for other combo
//      sensors.
//  2. Add fields to CC1101DTA to hold the readings (ie. wind speed)
//  3. Change the decode procedure to correctly verify and decode new
//      sensor data
// Adding another sensor with a different protocol is HARD:
//  1. All of the above PLUS you have to research the radio protocol and
//      make sure to tune the chip accordingly. You might want to use
//      the TX function to compare your setup with the existing sensor
//      using the oscilloscope. This is certainly not an easy task and
//      a lot of trial and error is involved.

struct Receiver {
	Receiver(CC1101 *receiver, SysSem *semaphore, string *jsonFileName)
		: receiver(receiver), semaphore(semaphore), jsonFileName(jsonFileName) {}
  CC1101 *receiver;
	SysSem *semaphore;
  string *jsonFileName;
};
static vector<Receiver *> receivers;

static void wiringPiCallback() {
  // go over all the radios; if you want the second board it is OK to
  // to share the interrupt pin because chip has enough information for
  // the program to decide whether packet was received or not
  // In other words, it's OK to call board's callback even if packet
  // in fact wasn't received on this particular board
#ifdef CC1101_DEBUG_LOGGING
  syslog(LOG_MAKEPRI(LOG_USER, LOG_INFO), "callbackFunction: called");
#endif
  for (Receiver * r : receivers) {
    try {
      // CC1101's callback doesn't print anything or write a JSON
      // all it does is it decodes packet and stores it along with a timestamp
      // in a circular buffer to be processed later. The rationale behind this
      // is that allows the system to catch a second copy that follows in a
      // rapid succession. This is not all that critically important but I
			// don't want to rewrite the mechanics.
      r->semaphore->lock();
      r->receiver->classCallback();
      r->semaphore->unlock();
    } catch (exception& e) {
			syslog(LOG_MAKEPRI(LOG_USER, LOG_ERR), "callback: %s", e.what());
      r->semaphore->unlock(); // this is critical; release SPI on error
      throw;
    }
  }
}
static vector<CC1101REG *> chipSetup {
  // GDO0: mode 0x80; analog temperature sensor
  // ---
	// Use of temperature sensor is considered to minimize physical wiring in the
	// cabinet. This way you can get away with only CC1101 module and no BME
	// modules (with the obvious loss of precision of the specialized chip). Note
	// that temperature sensor is analog so you still need an external ADC and
	// reading/calibrating it is outside of this module.
  (new IOCFG0())->gdo0_cfg(0x80),

  // GDO2: mode 0x06; setup an interrupt on the INT_EDGE_FALLING
  // ---
  // Asserts when sync word has been sent / received, and de-asserts at
  // the end of the packet. In RX, the pin will also deassert when a packet
  // is discarded due to address or maximum length filtering or when the
  // radio enters RXFIFO_OVERFLOW state. In TX the pin will de-assert if
  // the TX FIFO underflows
  (new IOCFG2())->gdo2_cfg(0x06),

  // adc_retension=1 does not seem to matter; SmartRF suggests 1
  // (new FIFOTHR())->adc_retension(1),

  // packet_length=0x14: status byte + 19 bytes packet
  (new PKTLEN())->packet_length(0x13),

  // Sync word MSB; 0x55 for v2.1 sensors.
  // This one is tricky: Oregon Scientific uses 1 nibble (4 bit) sync
  // word; for v2.1 sensors with every bit inverted-doubled it becomes
  // 1 byte (0x99). So I use 1 byte of preamble as a sync word MSB and
  // shorten the preamble by one
  // UPD: this may not be entirely true. What's here works for me now.
  (new SYNC1())->sync(0x55),

  // Sync word LSB; 0x99 for v2.1 sensors (see above)
  (new SYNC0())->sync(0x99),

  // pqt=3 filters out noise; see the TI document on how does it work
  // but you can use pqt=1..3 with 1 gives more garbage than 2 and so on
  // pqt=0 turn filtering off and gives alot of garbage (that can be
  // filtered out later using decode, chkSum and CRC8 methods)
  (new PKTCTRL1())->pqt(3),

  // this is packet format: no data whitening, no CRC, fixed
  // packet length
  (new PKTCTRL0())->white_data(0)->crc_en(0)->length_config(0),

  // IF setup. SmartRF suggests 0x06. Does not seem to matter.
  (new FSCTRL1())->freq_if(0x06),

  // Base frequency setup; official frequency is 433.92 MHz. You might
  // get a little better result if tuned slightly off-center. YMMV.
  (new FREQ2())->freq(0x10),
  (new FREQ1())->freq(0xb0),
  (new FREQ0())->freq(0x71),

  // Use PA entry #1 if you want to transmit and pretend to be OS weather
  // sensor yourself. Has no effect while RX-only.
  // c1101setup.push_back((new FREND0())->pa_power(0x01));

  // channel bandwidth: don't set it too narrow, on 3:3 you won't get
  // anything at all. 3:0 seems to have good enough balance between noise
  // and real signal
  // data rate: official data rate is 1024 bits/s, however Manchester makes
  // it doubled, 2048 bits/s. Don't set it any less than 2048 or you won't
  // get anything at all. Don't set it too high either or you get garbage.
  (new MDMCFG4())->chanbw_e(3)->chanbw_m(0)->drate_e(0x06),
  (new MDMCFG3())->drate_m(0x4a),

  // modulation/manchester/sync mode; not much you can tune here
  (new MDMCFG2())->mod_format(3)->sync_mode(6)->manchester_en(1),

  // preamble length; it seems like it should be 1 (3 bytes) but
  // if you try to transmit and compare the packet on the oscilloscope
  // to the original sensor then 3 bytes give you much shorter preamble
  // than expected. Use 2, don't change.
  (new MDMCFG1())->num_preamble(2),

  // auto-calibrate FS when going from IDLE to TX/RX. This is The MUST.
  (new MCSM0())->fs_autocal(1)->po_timeout(3),

  // any other parameters you want to change go here. Feel free to experiment.

  // end of chip setup
};
void jsonUpdateLoop() {
	for (Receiver * r : receivers) {
#ifdef CC1101_DEBUG_LOGGING
		// TODO proper semaphore should be used for getSetup()
		syslog(LOG_MAKEPRI(LOG_USER, LOG_INFO), "--- mark %s ---",
				r->receiver->getSetup());
#endif
		vector<CC1101DTA *> packets;
		for (int i = 0; i < CC1101_NBUFFERS; i++) {
			CC1101DTA * data = r->receiver->getPacket(i);
			if (data->tmstamp == 0) {
				goto skipAddition; // packet is empty/invalid
			}
			for (unsigned j = 0; j < packets.size(); j++) {
				if (data->getRCID() == packets[j]->getRCID()) {
					if (data->tmstamp > packets[j]->tmstamp) {
						packets[j] = data;
					}
					goto skipAddition;
				}
			}
			packets.push_back(data);
skipAddition:
			; // do nothing if RCID was present and packet already got updated
		}

		char newFile[PATH_MAX];
		snprintf(newFile, sizeof newFile,
				"%s.new", r->jsonFileName->c_str());

		FILE *json = fopen(newFile, "w");
		if (json == NULL) {
			syslog(LOG_MAKEPRI(LOG_USER, LOG_ERR),
					"can't open JSON file: %s", newFile);
			throw runtime_error("can't open JSON file");
		} else {
			fprintf(json, "{");
			for (CC1101DTA * packet : packets) {
				syslog(LOG_MAKEPRI(LOG_USER, LOG_INFO), "%s", packet->toString());
				fprintf(json, "%s", packet->toJSON());
			}
			time_t tmstamp = time(NULL);
			char finalTimestamp[256];
			strftime(finalTimestamp, sizeof finalTimestamp,
					"\"timestamp\": \"%FT%TZ\"", localtime(&tmstamp));
			fprintf(json, "%s}\n", finalTimestamp); // close JSON & newline
      fsync(fileno(json));
			fclose(json);
			rename(newFile, r->jsonFileName->c_str());
		}
	}
}
int main(int argc, char *argv[]) {
  SysSem semaphore("/spi"); // use SPI bus

  openlog(semaphore.imgName(),
		LOG_PERROR|LOG_CONS|LOG_PID|LOG_NDELAY, LOG_USER);
  syslog(LOG_MAKEPRI(LOG_USER, LOG_INFO), "%s", PACKAGE_STRING);

  if (wiringPiSetupSys() == -1) {
    const char * error = "can't setup wiringPi Sys mode";
    syslog(LOG_MAKEPRI(LOG_USER, LOG_ERR), "%s", error);
    throw runtime_error(error);
  }

  SPISETUP	spi;
  int polltime = 30; // some oversampling; each sensor fires ~once a minute

  while (true) {
    static const struct option long_options[] = {
      {"channel",	required_argument,  0, 'c', },
      {"speed",   required_argument,  0, 's', },
      {"gdo0",    required_argument,  0, '0', },
      {"gdo2",    required_argument,  0, '2', },
      {"polltime",required_argument,  0, 'p' },
      {"jsonfile",required_argument,  0, 'j', },
      {"help",    no_argument,        0, 'h', },
      {0,         0,                  0,   0, },
    };
    int c = getopt_long(argc, argv, "c:s:0:2:p:j:h", long_options, NULL);
    if (c == -1) {
      break;
    }
    switch (c) {
      case 'c':
        spi.channel = strtol(optarg, NULL, 0);
        break;
      case 's':
        spi.speed = strtol(optarg, NULL, 0);
        break;
      case '0':
        spi.gdo0 = strtol(optarg, NULL, 0);
        break;
      case '2':
        spi.gdo2 = strtol(optarg, NULL, 0);
        break;
      case 'p':
        polltime = strtol(optarg, NULL, 0);
        break;
      case 'j':
				// TODO change CC1101 to make a copy of SPISETUP
				receivers.push_back(new Receiver(
							new CC1101(spi, chipSetup), &semaphore, new string(optarg)));
				break;
      case 'h':
      default:
        printf("Usage: %s --polltime arg --channel arg", argv[0]);
        printf(" --speed arg --gdo0 arg --gdo2 arg --jsonfile arg\n");
        printf("\t[...]\n");
        exit(0);
    }
  }
  try {
		for (Receiver * r : receivers) {
			semaphore.lock();
			r->receiver->setup();
			wiringPiISR(r->receiver->spi.gdo2, INT_EDGE_FALLING, wiringPiCallback);
			r->receiver->srx(); // enable radio
			semaphore.unlock();
		}
    while (receivers.size() != 0) {
      sleep(polltime);
      jsonUpdateLoop();
    }
  } catch (exception& e) {
    semaphore.unlock();
    syslog(LOG_MAKEPRI(LOG_USER, LOG_ERR), "%s", e.what());
  }
  return 0;
}
