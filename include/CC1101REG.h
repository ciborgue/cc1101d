#ifndef __CC1101REG_H__
#define __CC1101REG_H__
#include <stdint.h>
#include <stdexcept>

using namespace std;

class CC1101REG {
  protected:
    uint8_t data;

  public:
    const uint8_t SIZE, CODE;

    CC1101REG(const uint8_t size, const uint8_t code)
      : SIZE(size), CODE(code) {
      data = 0x00;
    }
    uint8_t getData() {
      if (SIZE == 0) {
        throw invalid_argument("can't get data from command strobe") ;
      }
      return data;
    }
    CC1101REG * setData(const uint8_t newData) {
      if (SIZE == 0) {
        throw invalid_argument("can't set data for command strobe") ;
      }
      data = newData;
      return this;
    }
};
// ----------------------------------------
class IOCFG2: public CC1101REG {
  public:
    IOCFG2() : CC1101REG(1, 0x00) {
      gdo2_cfg(0x29);
    }
    IOCFG2 * gdo2_inv(const uint8_t c) {
      data ^= data & (0x01 << 6) ;
      data |= (c & 0x01) << 6;
      return this;
    }
    uint8_t gdo2_inv() {
      return (data >> 6) & 0x01;
    }
    IOCFG2 * gdo2_cfg(const uint8_t c) {
      data ^= data & 0x3f ;
      data |= c & 0x3f ;
      return this;
    }
    uint8_t gdo2_cfg() {
      return data & 0x3f;
    }
};
class IOCFG1: public CC1101REG {
  public:
    IOCFG1() : CC1101REG(1, 0x01) {
      gdo1_cfg(0x2e);
    }
    IOCFG1 * gdo_ds(const uint8_t c) {
      data ^= data & (0x01 << 7) ;
      data |= (c & 0x01) << 7;
      return this;
    }
    uint8_t gdo_ds() {
      return (data >> 7) & 0x01;
    }
    IOCFG1 * gdo1_inv(const uint8_t c) {
      data ^= data & (0x01 << 6) ;
      data |= (c & 0x01) << 6;
      return this;
    }
    uint8_t gdo1_inv() {
      return (data >> 6) & 0x01;
    }
    IOCFG1 * gdo1_cfg(const uint8_t c) {
      data ^= data & 0x3f ;
      data |= c & 0x3f ;
      return this;
    }
    uint8_t gdo1_cfg() {
      return data & 0x3f;
    }
};
class IOCFG0: public CC1101REG {
  public:
    IOCFG0() : CC1101REG(1, 0x02) {
      gdo0_cfg(0x3f);
    }
    IOCFG0 * temp_sensors_enable(const uint8_t c) {
      data ^= data & (0x01 << 7) ;
      data |= (c & 0x01) << 7;
      return this;
    }
    uint8_t temp_sensors_enable() {
      return (data >> 7) & 0x01;
    }
    IOCFG0 * gdo0_inv(const uint8_t c) {
      data ^= data & (0x01 << 6) ;
      data |= (c & 0x01) << 6;
      return this;
    }
    uint8_t gdo0_inv() {
      return (data >> 6) & 0x01;
    }
    IOCFG0 * gdo0_cfg(const uint8_t c) {
      data ^= data & 0x3f ;
      data |= c & 0x3f ;
      return this;
    }
    uint8_t gdo0_cfg() {
      return data & 0x3f;
    }
};
class FIFOTHR: public CC1101REG {
  public:
    FIFOTHR() : CC1101REG(1, 0x03) {
      fifo_thr(0x07);
    }
    FIFOTHR * adc_retension(const uint8_t c) {
      data ^= data & (0x01 << 6) ;
      data |= (c & 0x01) << 6;
      return this;
    }
    uint8_t adc_retension() {
      return (data >> 6) & 0x01;
    }
    FIFOTHR * close_in_rx(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t close_in_rx() {
      return (data >> 4) & 0x03;
    }
    FIFOTHR * fifo_thr(const uint8_t c) {
      data ^= data & 0x0f ;
      data |= c & 0x0f ;
      return this;
    }
    uint8_t fifo_thr() {
      return data & 0x0f;
    }
};
class SYNC1: public CC1101REG {
  public:
    SYNC1() : CC1101REG(1, 0x04) {
      sync(0xd3);
    }
    SYNC1 * sync(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t sync() {
      return data ;
    }
};
class SYNC0: public CC1101REG {
  public:
    SYNC0() : CC1101REG(1, 0x05) {
      sync(0x91);
    }
    SYNC0 * sync(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t sync() {
      return data ;
    }
};
class PKTLEN: public CC1101REG {
  public:
    PKTLEN() : CC1101REG(1, 0x06) {
      packet_length(0xff);
    }
    PKTLEN * packet_length(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t packet_length() {
      return data ;
    }
};
class PKTCTRL1: public CC1101REG {
  public:
    PKTCTRL1() : CC1101REG(1, 0x07) {
      append_status(0x01);
    }
    PKTCTRL1 * pqt(const uint8_t c) {
      data ^= data & (0x07 << 5) ;
      data |= (c & 0x07) << 5;
      return this;
    }
    uint8_t pqt() {
      return (data >> 5) & 0x07;
    }
    PKTCTRL1 * crc_autoflush(const uint8_t c) {
      data ^= data & (0x01 << 3) ;
      data |= (c & 0x01) << 3;
      return this;
    }
    uint8_t crc_autoflush() {
      return (data >> 3) & 0x01;
    }
    PKTCTRL1 * append_status(const uint8_t c) {
      data ^= data & (0x01 << 2) ;
      data |= (c & 0x01) << 2;
      return this;
    }
    uint8_t append_status() {
      return (data >> 2) & 0x01;
    }
    PKTCTRL1 * adr_chk(const uint8_t c) {
      data ^= data & 0x03 ;
      data |= c & 0x03 ;
      return this;
    }
    uint8_t adr_chk() {
      return data & 0x03;
    }
};
class PKTCTRL0: public CC1101REG {
  public:
    PKTCTRL0() : CC1101REG(1, 0x08) {
      white_data(0x01);
      crc_en(0x01);
      length_config(0x01);
    }
    PKTCTRL0 * white_data(const uint8_t c) {
      data ^= data & (0x01 << 6) ;
      data |= (c & 0x01) << 6;
      return this;
    }
    uint8_t white_data() {
      return (data >> 6) & 0x01;
    }
    PKTCTRL0 * pkt_format(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t pkt_format() {
      return (data >> 4) & 0x03;
    }
    PKTCTRL0 * crc_en(const uint8_t c) {
      data ^= data & (0x01 << 2) ;
      data |= (c & 0x01) << 2;
      return this;
    }
    uint8_t crc_en() {
      return (data >> 2) & 0x01;
    }
    PKTCTRL0 * length_config(const uint8_t c) {
      data ^= data & 0x03 ;
      data |= c & 0x03 ;
      return this;
    }
    uint8_t length_config() {
      return data & 0x03;
    }
};
class ADDR: public CC1101REG {
  public:
    ADDR() : CC1101REG(1, 0x09) {
    }
    ADDR * device_addr(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t device_addr() {
      return data ;
    }
};
class CHANNR: public CC1101REG {
  public:
    CHANNR() : CC1101REG(1, 0x0a) {
    }
    CHANNR * chan(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t chan() {
      return data ;
    }
};
class FSCTRL1: public CC1101REG {
  public:
    FSCTRL1() : CC1101REG(1, 0x0b) {
      freq_if(0x0f);
    }
    FSCTRL1 * freq_if(const uint8_t c) {
      data ^= data & 0x1f ;
      data |= c & 0x1f ;
      return this;
    }
    uint8_t freq_if() {
      return data & 0x1f;
    }
};
class FSCTRL0: public CC1101REG {
  public:
    FSCTRL0() : CC1101REG(1, 0x0c) {
    }
    FSCTRL0 * freqoff(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t freqoff() {
      return data ;
    }
};
class FREQ2: public CC1101REG {
  public:
    FREQ2() : CC1101REG(1, 0x0d) {
      freq(0x1e);
    }
    FREQ2 * freq(const uint8_t c) {
      data ^= data & 0x3f ;
      data |= c & 0x3f ;
      return this;
    }
    uint8_t freq() {
      return data & 0x3f;
    }
};
class FREQ1: public CC1101REG {
  public:
    FREQ1() : CC1101REG(1, 0x0e) {
      freq(0xc4);
    }
    FREQ1 * freq(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t freq() {
      return data ;
    }
};
class FREQ0: public CC1101REG {
  public:
    FREQ0() : CC1101REG(1, 0x0f) {
      freq(0xec);
    }
    FREQ0 * freq(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t freq() {
      return data ;
    }
};
class MDMCFG4: public CC1101REG {
  public:
    MDMCFG4() : CC1101REG(1, 0x10) {
      chanbw_e(0x02);
      drate_e(0x0c);
    }
    MDMCFG4 * chanbw_e(const uint8_t c) {
      data ^= data & (0x03 << 6) ;
      data |= (c & 0x03) << 6;
      return this;
    }
    uint8_t chanbw_e() {
      return (data >> 6) & 0x03;
    }
    MDMCFG4 * chanbw_m(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t chanbw_m() {
      return (data >> 4) & 0x03;
    }
    MDMCFG4 * drate_e(const uint8_t c) {
      data ^= data & 0x0f ;
      data |= c & 0x0f ;
      return this;
    }
    uint8_t drate_e() {
      return data & 0x0f;
    }
};
class MDMCFG3: public CC1101REG {
  public:
    MDMCFG3() : CC1101REG(1, 0x11) {
      drate_m(0x22);
    }
    MDMCFG3 * drate_m(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t drate_m() {
      return data ;
    }
};
class MDMCFG2: public CC1101REG {
  public:
    MDMCFG2() : CC1101REG(1, 0x12) {
      sync_mode(0x02);
    }
    MDMCFG2 * dem_dcfilt_off(const uint8_t c) {
      data ^= data & (0x01 << 7) ;
      data |= (c & 0x01) << 7;
      return this;
    }
    uint8_t dem_dcfilt_off() {
      return (data >> 7) & 0x01;
    }
    MDMCFG2 * mod_format(const uint8_t c) {
      data ^= data & (0x07 << 4) ;
      data |= (c & 0x07) << 4;
      return this;
    }
    uint8_t mod_format() {
      return (data >> 4) & 0x07;
    }
    MDMCFG2 * manchester_en(const uint8_t c) {
      data ^= data & (0x01 << 3) ;
      data |= (c & 0x01) << 3;
      return this;
    }
    uint8_t manchester_en() {
      return (data >> 3) & 0x01;
    }
    MDMCFG2 * sync_mode(const uint8_t c) {
      data ^= data & 0x07 ;
      data |= c & 0x07 ;
      return this;
    }
    uint8_t sync_mode() {
      return data & 0x07;
    }
};
class MDMCFG1: public CC1101REG {
  public:
    MDMCFG1() : CC1101REG(1, 0x13) {
      num_preamble(0x02);
      chanspc_e(0x02);
    }
    MDMCFG1 * fec_en(const uint8_t c) {
      data ^= data & (0x01 << 7) ;
      data |= (c & 0x01) << 7;
      return this;
    }
    uint8_t fec_en() {
      return (data >> 7) & 0x01;
    }
    MDMCFG1 * num_preamble(const uint8_t c) {
      data ^= data & (0x07 << 4) ;
      data |= (c & 0x07) << 4;
      return this;
    }
    uint8_t num_preamble() {
      return (data >> 4) & 0x07;
    }
    MDMCFG1 * chanspc_e(const uint8_t c) {
      data ^= data & 0x03 ;
      data |= c & 0x03 ;
      return this;
    }
    uint8_t chanspc_e() {
      return data & 0x03;
    }
};
class MDMCFG0: public CC1101REG {
  public:
    MDMCFG0() : CC1101REG(1, 0x14) {
      chanspc_m(0xf8);
    }
    MDMCFG0 * chanspc_m(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t chanspc_m() {
      return data ;
    }
};
class DEVIATN: public CC1101REG {
  public:
    DEVIATN() : CC1101REG(1, 0x15) {
      deviation_e(0x04);
      deviation_m(0x07);
    }
    DEVIATN * deviation_e(const uint8_t c) {
      data ^= data & (0x07 << 4) ;
      data |= (c & 0x07) << 4;
      return this;
    }
    uint8_t deviation_e() {
      return (data >> 4) & 0x07;
    }
    DEVIATN * deviation_m(const uint8_t c) {
      data ^= data & 0x07 ;
      data |= c & 0x07 ;
      return this;
    }
    uint8_t deviation_m() {
      return data & 0x07;
    }
};
class MCSM2: public CC1101REG {
  public:
    MCSM2() : CC1101REG(1, 0x16) {
      rx_time(0x07);
    }
    MCSM2 * rx_time_rssi(const uint8_t c) {
      data ^= data & (0x01 << 4) ;
      data |= (c & 0x01) << 4;
      return this;
    }
    uint8_t rx_time_rssi() {
      return (data >> 4) & 0x01;
    }
    MCSM2 * rx_time_qual(const uint8_t c) {
      data ^= data & (0x01 << 3) ;
      data |= (c & 0x01) << 3;
      return this;
    }
    uint8_t rx_time_qual() {
      return (data >> 3) & 0x01;
    }
    MCSM2 * rx_time(const uint8_t c) {
      data ^= data & 0x07 ;
      data |= c & 0x07 ;
      return this;
    }
    uint8_t rx_time() {
      return data & 0x07;
    }
};
class MCSM1: public CC1101REG {
  public:
    MCSM1() : CC1101REG(1, 0x17) {
      cca_mode(0x03);
    }
    MCSM1 * cca_mode(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t cca_mode() {
      return (data >> 4) & 0x03;
    }
    MCSM1 * rxoff_mode(const uint8_t c) {
      data ^= data & (0x03 << 2) ;
      data |= (c & 0x03) << 2;
      return this;
    }
    uint8_t rxoff_mode() {
      return (data >> 2) & 0x03;
    }
    MCSM1 * txoff_mode(const uint8_t c) {
      data ^= data & 0x03 ;
      data |= c & 0x03 ;
      return this;
    }
    uint8_t txoff_mode() {
      return data & 0x03;
    }
};
class MCSM0: public CC1101REG {
  public:
    MCSM0() : CC1101REG(1, 0x18) {
      po_timeout(0x01);
    }
    MCSM0 * fs_autocal(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t fs_autocal() {
      return (data >> 4) & 0x03;
    }
    MCSM0 * po_timeout(const uint8_t c) {
      data ^= data & (0x03 << 2) ;
      data |= (c & 0x03) << 2;
      return this;
    }
    uint8_t po_timeout() {
      return (data >> 2) & 0x03;
    }
    MCSM0 * pin_ctrl_en(const uint8_t c) {
      data ^= data & (0x01 << 1) ;
      data |= (c & 0x01) << 1;
      return this;
    }
    uint8_t pin_ctrl_en() {
      return (data >> 1) & 0x01;
    }
    MCSM0 * xosc_force_on(const uint8_t c) {
      data ^= data & 0x01 ;
      data |= c & 0x01 ;
      return this;
    }
    uint8_t xosc_force_on() {
      return data & 0x01;
    }
};
class FOCCFG: public CC1101REG {
  public:
    FOCCFG() : CC1101REG(1, 0x19) {
      foc_bs_cs_gate(0x01);
      foc_pre_k(0x02);
      foc_post_k(0x01);
      foc_limit(0x02);
    }
    FOCCFG * foc_bs_cs_gate(const uint8_t c) {
      data ^= data & (0x01 << 5) ;
      data |= (c & 0x01) << 5;
      return this;
    }
    uint8_t foc_bs_cs_gate() {
      return (data >> 5) & 0x01;
    }
    FOCCFG * foc_pre_k(const uint8_t c) {
      data ^= data & (0x03 << 3) ;
      data |= (c & 0x03) << 3;
      return this;
    }
    uint8_t foc_pre_k() {
      return (data >> 3) & 0x03;
    }
    FOCCFG * foc_post_k(const uint8_t c) {
      data ^= data & (0x01 << 2) ;
      data |= (c & 0x01) << 2;
      return this;
    }
    uint8_t foc_post_k() {
      return (data >> 2) & 0x01;
    }
    FOCCFG * foc_limit(const uint8_t c) {
      data ^= data & 0x03 ;
      data |= c & 0x03 ;
      return this;
    }
    uint8_t foc_limit() {
      return data & 0x03;
    }
};
class BSCFG: public CC1101REG {
  public:
    BSCFG() : CC1101REG(1, 0x1a) {
      bs_pre_ki(0x01);
      bs_pre_kp(0x02);
      bs_post_ki(0x01);
      bs_post_kp(0x01);
    }
    BSCFG * bs_pre_ki(const uint8_t c) {
      data ^= data & (0x03 << 6) ;
      data |= (c & 0x03) << 6;
      return this;
    }
    uint8_t bs_pre_ki() {
      return (data >> 6) & 0x03;
    }
    BSCFG * bs_pre_kp(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t bs_pre_kp() {
      return (data >> 4) & 0x03;
    }
    BSCFG * bs_post_ki(const uint8_t c) {
      data ^= data & (0x01 << 3) ;
      data |= (c & 0x01) << 3;
      return this;
    }
    uint8_t bs_post_ki() {
      return (data >> 3) & 0x01;
    }
    BSCFG * bs_post_kp(const uint8_t c) {
      data ^= data & (0x01 << 2) ;
      data |= (c & 0x01) << 2;
      return this;
    }
    uint8_t bs_post_kp() {
      return (data >> 2) & 0x01;
    }
    BSCFG * bs_limit(const uint8_t c) {
      data ^= data & 0x03 ;
      data |= c & 0x03 ;
      return this;
    }
    uint8_t bs_limit() {
      return data & 0x03;
    }
};
class AGCCTRL2: public CC1101REG {
  public:
    AGCCTRL2() : CC1101REG(1, 0x1b) {
      magn_target(0x03);
    }
    AGCCTRL2 * max_dvga_gain(const uint8_t c) {
      data ^= data & (0x03 << 6) ;
      data |= (c & 0x03) << 6;
      return this;
    }
    uint8_t max_dvga_gain() {
      return (data >> 6) & 0x03;
    }
    AGCCTRL2 * max_lna_gain(const uint8_t c) {
      data ^= data & (0x07 << 3) ;
      data |= (c & 0x07) << 3;
      return this;
    }
    uint8_t max_lna_gain() {
      return (data >> 3) & 0x07;
    }
    AGCCTRL2 * magn_target(const uint8_t c) {
      data ^= data & 0x07 ;
      data |= c & 0x07 ;
      return this;
    }
    uint8_t magn_target() {
      return data & 0x07;
    }
};
class AGCCTRL1: public CC1101REG {
  public:
    AGCCTRL1() : CC1101REG(1, 0x1c) {
      agc_lna_priority(0x01);
    }
    AGCCTRL1 * agc_lna_priority(const uint8_t c) {
      data ^= data & (0x01 << 6) ;
      data |= (c & 0x01) << 6;
      return this;
    }
    uint8_t agc_lna_priority() {
      return (data >> 6) & 0x01;
    }
    AGCCTRL1 * carrier_sense_rel_thr(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t carrier_sense_rel_thr() {
      return (data >> 4) & 0x03;
    }
    AGCCTRL1 * carrier_sense_abs_thr(const uint8_t c) {
      data ^= data & 0x0f ;
      data |= c & 0x0f ;
      return this;
    }
    uint8_t carrier_sense_abs_thr() {
      return data & 0x0f;
    }
};
class AGCCTRL0: public CC1101REG {
  public:
    AGCCTRL0() : CC1101REG(1, 0x1d) {
      hyst_level(0x02);
      wait_time(0x01);
      filter_length(0x01);
    }
    AGCCTRL0 * hyst_level(const uint8_t c) {
      data ^= data & (0x03 << 6) ;
      data |= (c & 0x03) << 6;
      return this;
    }
    uint8_t hyst_level() {
      return (data >> 6) & 0x03;
    }
    AGCCTRL0 * wait_time(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t wait_time() {
      return (data >> 4) & 0x03;
    }
    AGCCTRL0 * agc_freeze(const uint8_t c) {
      data ^= data & (0x03 << 2) ;
      data |= (c & 0x03) << 2;
      return this;
    }
    uint8_t agc_freeze() {
      return (data >> 2) & 0x03;
    }
    AGCCTRL0 * filter_length(const uint8_t c) {
      data ^= data & 0x03 ;
      data |= c & 0x03 ;
      return this;
    }
    uint8_t filter_length() {
      return data & 0x03;
    }
};
class WOREVT1: public CC1101REG {
  public:
    WOREVT1() : CC1101REG(1, 0x1e) {
      event0_15_08(0x87);
    }
    WOREVT1 * event0_15_08(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t event0_15_08() {
      return data ;
    }
};
class WOREVT0: public CC1101REG {
  public:
    WOREVT0() : CC1101REG(1, 0x1f) {
      event0_07_00(0x6b);
    }
    WOREVT0 * event0_07_00(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t event0_07_00() {
      return data ;
    }
};
class WORCTRL: public CC1101REG {
  public:
    WORCTRL() : CC1101REG(1, 0x20) {
      rc_pd(0x01);
      event1_02_00(0x07);
      rc_cal(0x01);
    }
    WORCTRL * rc_pd(const uint8_t c) {
      data ^= data & (0x01 << 7) ;
      data |= (c & 0x01) << 7;
      return this;
    }
    uint8_t rc_pd() {
      return (data >> 7) & 0x01;
    }
    WORCTRL * event1_02_00(const uint8_t c) {
      data ^= data & (0x07 << 4) ;
      data |= (c & 0x07) << 4;
      return this;
    }
    uint8_t event1_02_00() {
      return (data >> 4) & 0x07;
    }
    WORCTRL * rc_cal(const uint8_t c) {
      data ^= data & (0x01 << 3) ;
      data |= (c & 0x01) << 3;
      return this;
    }
    uint8_t rc_cal() {
      return (data >> 3) & 0x01;
    }
    WORCTRL * wor_res(const uint8_t c) {
      data ^= data & 0x03 ;
      data |= c & 0x03 ;
      return this;
    }
    uint8_t wor_res() {
      return data & 0x03;
    }
};
class FREND1: public CC1101REG {
  public:
    FREND1() : CC1101REG(1, 0x21) {
      lna_current(0x01);
      lna2mix_current(0x01);
      lodiv_buf_current_rx(0x01);
      mix_current(0x02);
    }
    FREND1 * lna_current(const uint8_t c) {
      data ^= data & (0x03 << 6) ;
      data |= (c & 0x03) << 6;
      return this;
    }
    uint8_t lna_current() {
      return (data >> 6) & 0x03;
    }
    FREND1 * lna2mix_current(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t lna2mix_current() {
      return (data >> 4) & 0x03;
    }
    FREND1 * lodiv_buf_current_rx(const uint8_t c) {
      data ^= data & (0x03 << 2) ;
      data |= (c & 0x03) << 2;
      return this;
    }
    uint8_t lodiv_buf_current_rx() {
      return (data >> 2) & 0x03;
    }
    FREND1 * mix_current(const uint8_t c) {
      data ^= data & 0x03 ;
      data |= c & 0x03 ;
      return this;
    }
    uint8_t mix_current() {
      return data & 0x03;
    }
};
class FREND0: public CC1101REG {
  public:
    FREND0() : CC1101REG(1, 0x22) {
      lodiv_buf_current_tx(0x01);
    }
    FREND0 * lodiv_buf_current_tx(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t lodiv_buf_current_tx() {
      return (data >> 4) & 0x03;
    }
    FREND0 * pa_power(const uint8_t c) {
      data ^= data & 0x07 ;
      data |= c & 0x07 ;
      return this;
    }
    uint8_t pa_power() {
      return data & 0x07;
    }
};
class FSCAL3: public CC1101REG {
  public:
    FSCAL3() : CC1101REG(1, 0x23) {
      fscal3_07_06(0x02);
      chp_curr_cal_en(0x02);
      fscal3_03_00(0x09);
    }
    FSCAL3 * fscal3_07_06(const uint8_t c) {
      data ^= data & (0x03 << 6) ;
      data |= (c & 0x03) << 6;
      return this;
    }
    uint8_t fscal3_07_06() {
      return (data >> 6) & 0x03;
    }
    FSCAL3 * chp_curr_cal_en(const uint8_t c) {
      data ^= data & (0x03 << 4) ;
      data |= (c & 0x03) << 4;
      return this;
    }
    uint8_t chp_curr_cal_en() {
      return (data >> 4) & 0x03;
    }
    FSCAL3 * fscal3_03_00(const uint8_t c) {
      data ^= data & 0x0f ;
      data |= c & 0x0f ;
      return this;
    }
    uint8_t fscal3_03_00() {
      return data & 0x0f;
    }
};
class FSCAL2: public CC1101REG {
  public:
    FSCAL2() : CC1101REG(1, 0x24) {
      fscal2_04_00(0x0a);
    }
    FSCAL2 * vco_core_h_en(const uint8_t c) {
      data ^= data & (0x01 << 5) ;
      data |= (c & 0x01) << 5;
      return this;
    }
    uint8_t vco_core_h_en() {
      return (data >> 5) & 0x01;
    }
    FSCAL2 * fscal2_04_00(const uint8_t c) {
      data ^= data & 0x1f ;
      data |= c & 0x1f ;
      return this;
    }
    uint8_t fscal2_04_00() {
      return data & 0x1f;
    }
};
class FSCAL1: public CC1101REG {
  public:
    FSCAL1() : CC1101REG(1, 0x25) {
      fscal1_05_00(0x20);
    }
    FSCAL1 * fscal1_05_00(const uint8_t c) {
      data ^= data & 0x3f ;
      data |= c & 0x3f ;
      return this;
    }
    uint8_t fscal1_05_00() {
      return data & 0x3f;
    }
};
class FSCAL0: public CC1101REG {
  public:
    FSCAL0() : CC1101REG(1, 0x26) {
      fscal0_06_00(0x0d);
    }
    FSCAL0 * fscal0_06_00(const uint8_t c) {
      data ^= data & 0x7f ;
      data |= c & 0x7f ;
      return this;
    }
    uint8_t fscal0_06_00() {
      return data & 0x7f;
    }
};
class RCCTRL1: public CC1101REG {
  public:
    RCCTRL1() : CC1101REG(1, 0x27) {
      rcctrl1(0x41);
    }
    RCCTRL1 * rcctrl1(const uint8_t c) {
      data ^= data & 0x7f ;
      data |= c & 0x7f ;
      return this;
    }
    uint8_t rcctrl1() {
      return data & 0x7f;
    }
};
class RCCTRL0: public CC1101REG {
  public:
    RCCTRL0() : CC1101REG(1, 0x28) {
    }
    RCCTRL0 * rcctrl0(const uint8_t c) {
      data ^= data & 0x7f ;
      data |= c & 0x7f ;
      return this;
    }
    uint8_t rcctrl0() {
      return data & 0x7f;
    }
};
class FSTEST: public CC1101REG {
  public:
    FSTEST() : CC1101REG(1, 0x29) {
      fstest(0x59);
    }
    FSTEST * fstest(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t fstest() {
      return data ;
    }
};
class PTEST: public CC1101REG {
  public:
    PTEST() : CC1101REG(1, 0x2a) {
      ptest(0x7f);
    }
    PTEST * ptest(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t ptest() {
      return data ;
    }
};
class AGCTEST: public CC1101REG {
  public:
    AGCTEST() : CC1101REG(1, 0x2b) {
      agctest(0x3f);
    }
    AGCTEST * agctest(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t agctest() {
      return data ;
    }
};
class TEST2: public CC1101REG {
  public:
    TEST2() : CC1101REG(1, 0x2c) {
      test2(0x88);
    }
    TEST2 * test2(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t test2() {
      return data ;
    }
};
class TEST1: public CC1101REG {
  public:
    TEST1() : CC1101REG(1, 0x2d) {
      test1(0x31);
    }
    TEST1 * test1(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t test1() {
      return data ;
    }
};
class TEST0: public CC1101REG {
  public:
    TEST0() : CC1101REG(1, 0x2e) {
      test0_07_02(0x02);
      vco_sel_cal_en(0x01);
      test0_00(0x01);
    }
    TEST0 * test0_07_02(const uint8_t c) {
      data ^= data & (0x3f << 2) ;
      data |= (c & 0x3f) << 2;
      return this;
    }
    uint8_t test0_07_02() {
      return (data >> 2) & 0x3f;
    }
    TEST0 * vco_sel_cal_en(const uint8_t c) {
      data ^= data & (0x01 << 1) ;
      data |= (c & 0x01) << 1;
      return this;
    }
    uint8_t vco_sel_cal_en() {
      return (data >> 1) & 0x01;
    }
    TEST0 * test0_00(const uint8_t c) {
      data ^= data & 0x01 ;
      data |= c & 0x01 ;
      return this;
    }
    uint8_t test0_00() {
      return data & 0x01;
    }
};
class UNUSED0x2F: public CC1101REG {
  public:
    UNUSED0x2F() : CC1101REG(1, 0x2f) {
    }
    UNUSED0x2F * unused0x2f(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t unused0x2f() {
      return data ;
    }
};
class SRES: public CC1101REG {
  public:
    SRES() : CC1101REG(0, 0x30) {
    }
};
class SFSTXON: public CC1101REG {
  public:
    SFSTXON() : CC1101REG(0, 0x31) {
    }
};
class SXOFF: public CC1101REG {
  public:
    SXOFF() : CC1101REG(0, 0x32) {
    }
};
class SCAL: public CC1101REG {
  public:
    SCAL() : CC1101REG(0, 0x33) {
    }
};
class SRX: public CC1101REG {
  public:
    SRX() : CC1101REG(0, 0x34) {
    }
};
class STX: public CC1101REG {
  public:
    STX() : CC1101REG(0, 0x35) {
    }
};
class SIDLE: public CC1101REG {
  public:
    SIDLE() : CC1101REG(0, 0x36) {
    }
};
class UNUSED0x37: public CC1101REG {
  public:
    UNUSED0x37() : CC1101REG(0, 0x37) {
    }
};
class SWOR: public CC1101REG {
  public:
    SWOR() : CC1101REG(0, 0x38) {
    }
};
class SPWD: public CC1101REG {
  public:
    SPWD() : CC1101REG(0, 0x39) {
    }
};
class SFRX: public CC1101REG {
  public:
    SFRX() : CC1101REG(0, 0x3a) {
    }
};
class SFTX: public CC1101REG {
  public:
    SFTX() : CC1101REG(0, 0x3b) {
    }
};
class SWORRST: public CC1101REG {
  public:
    SWORRST() : CC1101REG(0, 0x3c) {
    }
};
class SNOP: public CC1101REG {
  public:
    SNOP() : CC1101REG(0, 0x3d) {
    }
};
class SNOPR: public CC1101REG {
  public:
    SNOPR() : CC1101REG(0, 0xbd) {
    }
};
class PARTNUM: public CC1101REG {
  public:
    PARTNUM() : CC1101REG(1, 0xf0) {
    }
    PARTNUM * partnum(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t partnum() {
      return data ;
    }
};
class VER: public CC1101REG {
  public:
    VER() : CC1101REG(1, 0xf1) {
    }
    VER * version(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t version() {
      return data ;
    }
};
class FREQUEST: public CC1101REG {
  public:
    FREQUEST() : CC1101REG(1, 0xf2) {
    }
    FREQUEST * frequest(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t frequest() {
      return data ;
    }
};
class LQI: public CC1101REG {
  public:
    LQI() : CC1101REG(1, 0xf3) {
    }
    LQI * lqi(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t lqi() {
      return data ;
    }
};
class RSSI: public CC1101REG {
  public:
    RSSI() : CC1101REG(1, 0xf4) {
    }
    RSSI * rssi(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t rssi() {
      return data ;
    }
};
class MARCSTATE: public CC1101REG {
  public:
    MARCSTATE() : CC1101REG(1, 0xf5) {
    }
    MARCSTATE * marcstate(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t marcstate() {
      return data ;
    }
};
class WORTIME1: public CC1101REG {
  public:
    WORTIME1() : CC1101REG(1, 0xf6) {
    }
    WORTIME1 * wortime1(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t wortime1() {
      return data ;
    }
};
class WORTIME0: public CC1101REG {
  public:
    WORTIME0() : CC1101REG(1, 0xf7) {
    }
    WORTIME0 * wortime0(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t wortime0() {
      return data ;
    }
};
class PKTSTATUS: public CC1101REG {
  public:
    PKTSTATUS() : CC1101REG(1, 0xf8) {
    }
    PKTSTATUS * pktstatus(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t pktstatus() {
      return data ;
    }
};
class VCO_DC_DAC: public CC1101REG {
  public:
    VCO_DC_DAC() : CC1101REG(1, 0xf9) {
    }
    VCO_DC_DAC * vco_dc_dac(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t vco_dc_dac() {
      return data ;
    }
};
class TXBYTES: public CC1101REG {
  public:
    TXBYTES() : CC1101REG(1, 0xfa) {
    }
    TXBYTES * txfifo_underflow(const uint8_t c) {
      data ^= data & (0x01 << 7) ;
      data |= (c & 0x01) << 7;
      return this;
    }
    uint8_t txfifo_underflow() {
      return (data >> 7) & 0x01;
    }
    TXBYTES * num_txbytes(const uint8_t c) {
      data ^= data & 0x7f ;
      data |= c & 0x7f ;
      return this;
    }
    uint8_t num_txbytes() {
      return data & 0x7f;
    }
};
class RXBYTES: public CC1101REG {
  public:
    RXBYTES() : CC1101REG(1, 0xfb) {
    }
    RXBYTES * rxfifo_overflow(const uint8_t c) {
      data ^= data & (0x01 << 7) ;
      data |= (c & 0x01) << 7;
      return this;
    }
    uint8_t rxfifo_overflow() {
      return (data >> 7) & 0x01;
    }
    RXBYTES * num_rxbytes(const uint8_t c) {
      data ^= data & 0x7f ;
      data |= c & 0x7f ;
      return this;
    }
    uint8_t num_rxbytes() {
      return data & 0x7f;
    }
};
class RCCTRL1_STATUS: public CC1101REG {
  public:
    RCCTRL1_STATUS() : CC1101REG(1, 0xfc) {
    }
    RCCTRL1_STATUS * rcctrl1_status(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t rcctrl1_status() {
      return data ;
    }
};
class RCCTRL0_STATUS: public CC1101REG {
  public:
    RCCTRL0_STATUS() : CC1101REG(1, 0xfd) {
    }
    RCCTRL0_STATUS * rcctrl0_status(const uint8_t c) {
      data = c ;
      return this;
    }
    uint8_t rcctrl0_status() {
      return data ;
    }
};
#endif // __CC1101REG_H__
