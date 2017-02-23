#include <vector>

#include "SPISETUP.h"

#include "CC1101REG.h"
#include "CC1101DTA.h"

using namespace std;

// CC1101_NBUFFERS: number of FIFO-size buffers in the circular storage
#ifndef CC1101_NBUFFERS
#define CC1101_NBUFFERS       16
#endif

// CC1101_DEBUG_LOGGING: log debugging events to syslog
// #define CC1101_DEBUG_LOGGING

class CC1101 {
  private:
		CC1101DTA	dataCollected[CC1101_NBUFFERS];
		int				dataCollectedIdx = 0;

		char text[256];

		void waitForChipRdy(bool ignoreChipRdy = false);

    void validateGDO0();
    void validateGDO2();
    void checkWiring();

  public:
    CC1101(SPISETUP &spi, vector<CC1101REG *> &chipSetup);
    ~CC1101();

    SPISETUP						&spi;
    vector<CC1101REG *>	&chipSetup;

		CC1101DTA	*getPacket(int index);

    int resetChip(bool ignoreChipRdy = false);

    void loadReg(CC1101REG *reg);
    int saveReg(CC1101REG *reg);

		void classCallback();

		const char *getSetup();
		void setup();
		void srx();
};
