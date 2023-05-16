#include "esphome/core/log.h"
#include "si1132.h"
#include "esphome/core/hal.h"

// BASED ON https://github.com/FaBoPlatform/FaBoUV-Si1132-Library/tree/master/src

/** si1132 Slave Address register */
#define SI1132_SLAVE_ADDRESS 0x60
/** Who_am_i register */
#define SI1132_WHO_AM_I_REG 0x00
/** Device check register */
#define SI1132_DEVICE 0x32

/** HW_KEY register */
#define SI1132_HW_KEY_REG 0x07
/** MEASRATE0 register */
#define SI1132_MEASRATE0_REG 0x08
/** MEASRATE1 register */
#define SI1132_MEASRATE1_REG 0x09
/** UCOEF0 register */
#define SI1132_UCOEF0_REG 0x13
/** UCOEF1 register */
#define SI1132_UCOEF1_REG 0x14
/** UCOEF2 register */
#define SI1132_UCOEF2_REG 0x15
/** UCOEF3 register */
#define SI1132_UCOEF3_REG 0x16
/** PARAM_WR register */
#define SI1132_PARAM_WR_REG 0x17
/** Command register */
#define SI1132_COMMAND_REG 0x18
/** Visible Data register */
#define SI1132_VISIBLE_DATA_REG 0x22
/** IR Data register */
#define SI1132_IR_DATA_REG 0x24
/** Auxiliary Data register */
#define SI1132_AUX_DATA_REG 0x2C

/** Chip list Parameter RAM Offset */
#define SI1132_CHIPLIST_PARAM_OFFSET 0x01
/** ALS Encoding Parameter RAM Offset */
#define SI1132_ALS_ENCODING_PARAM_OFFSET 0x06
/** ALS VIS ADC Counter register */
#define SI1132_ALS_VIS_ADC_COUNTER_PARAM_OFFSET 0x10
/** ALS VIS ADC Gain Parameter RAM Offset */
#define SI1132_ALS_VIS_ADC_GAIN_PARAM_OFFSET 0x11
/** ALS VIS ADC Misc Parameter RAM Offset */
#define SI1132_ALS_VIS_ADC_MISC_PARAM_OFFSET 0x12
/** ALS IR ADC Counter  Parameter RAM Offset */
#define SI1132_ALS_IR_ADC_COUNTER_PARAM_OFFSET 0x1D
/** ALS IR ADC Gain  Parameter RAM Offset */
#define SI1132_ALS_IR_ADC_GAIN_PARAM_OFFSET 0x1E
/** ALS IR ADC Misc  Parameter RAM Offset */
#define SI1132_ALS_IR_ADC_MISC_PARAM_OFFSET 0x1F
/** ALS IR Adcmux  Parameter RAM Offset */
#define SI1132_ALS_IR_ADCMUX_PARAM_OFFSET 0x0E
/** Auxiliary Adcmux  Parameter RAM Offset */
#define SI1132_AUX_ADCMUX_PARAM_OFFSET 0x0F

/** Enables UV Index register */
#define SI1132_EN_UV      0b10000000
/** Enables Auxiliary Channel register */
#define SI1132_EN_AUX     0b01000000
/** Enables ALS IR Channel register */
#define SI1132_EN_ALS_IR  0b00100000
/** Enables ALS Visible Channel register */
#define SI1132_EN_ALS_VIS 0b00010000

/** ALS　VIS ALIGN register */
#define SI1132_ALS_VIS_ALIGN 0b00010000
/** ALS　IR ALIGN register */
#define SI1132_ALS_IR_ALIGN  0b00100000

/** ADC Clock 1 : 50 ns times */
#define SI1132_1_ADC_CLOCK   0b00000000
/** ADC Clock 7 : 350 ns times */
#define SI1132_7_ADC_CLOCK   0b00010000
/** ADC Clock 15 : 750 ns times */
#define SI1132_15_ADC_CLOCK  0b00100000
/** ADC Clock 31 : 1.15 us times */
#define SI1132_31_ADC_CLOCK  0b00110000
/** ADC Clock 63 : 3.15 us times */
#define SI1132_63_ADC_CLOCK  0b01000000
/** ADC Clock 127 : 6.35 us times */
#define SI1132_127_ADC_CLOCK 0b10100000
/** ADC Clock 255 : 12.75 us times */
#define SI1132_255_ADC_CLOCK 0b01100000
/** ADC Clock 511 : 25.55 us times */
#define SI1132_511_ADC_CLOCK 0b01110000

/** Divided ADC Clock 1 */
#define SI1132_1_DIVIDED_ADC_CLOCK  0b0000000
/** Divided ADC Clock 16 */
#define SI1132_16_DIVIDED_ADC_CLOCK 0b0000100
/** Divided ADC Clock 32 */
#define SI1132_32_DIVIDED_ADC_CLOCK 0b0000101
/** Divided ADC Clock 64 */
#define SI1132_64_DIVIDED_ADC_CLOCK 0b0000110

/** Normal single range */
#define SI1132_NORMAL_SIGNAL_RANGE 0b00000000
/** High single range */
#define SI1132_HIGH_SIGNAL_RANGE   0b00100000

/** ALS IR Adcmux SMALLIR */
#define SI1132_ALS_IR_ADCMUX_SMALLIR 0x00

/** ALS IR Adcmux Temperature */
#define SI1132_AUX_ADCMUX_TEMPERATURE 0x65
/** Auxiliary ADCMUX VDD */
#define SI1132_AUX_ADCMUX_VDD 0x75

/** Command ALS Auto */
#define SI1132_COMMAND_ALS_AUTO 0x0E
/** Command ALS force */
#define SI1132_COMMAND_ALS_FORCE 0x06
/** Command Reset */
#define SI1132_COMMAND_RESET 0x01
/** HW_KEY Default Value */
#define SI1132_HW_KEY_DEFAULT 0x17

/** Normal single range */
#define SI1132_PARAM_QUERY 0b10000000
/** High single range */
#define SI1132_PARAM_SET   0b10100000


namespace esphome {
namespace si1132 {

static const char *TAG = "si1132.sensor";

float SI1132Component::get_setup_priority() const { return setup_priority::DATA; }

void SI1132Component::setup(){
  // Reset
  this->write_byte(SI1132_COMMAND_REG, SI1132_COMMAND_RESET);
  delay(1);
  this->write_byte(0x22, 0x01);
  delay(100);
  
  // Select HW_KEY register
  this->write_byte(SI1132_HW_KEY_REG, SI1132_HW_KEY_DEFAULT);

  // SET PARAM_WR(Chiplist)
  this->write_byte(SI1132_PARAM_WR_REG, SI1132_EN_ALS_IR|SI1132_EN_ALS_VIS); // Add SI1132_EN_UV if you need the (bad) UV estimation
  // COMMAND(Set Chiplist)
  this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_CHIPLIST_PARAM_OFFSET);

  // Rate setting
  //this->write_byte(SI1132_MEASRATE1_REG, 0x0);
  ///////////////////////////this->write_byte(SI1132_MEASRATE0_REG, 0x10);//ff);

  // SET PARAM_WR(ALS_ENCODING)
  //this->write_byte(SI1132_PARAM_WR_REG, SI1132_ALS_VIS_ALIGN | SI1132_ALS_IR_ALIGN); // Uncomment to read LSBs from the 13 bit ADC
  //this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_ENCODING_PARAM_OFFSET);


  /* IR */
  // SET ALS_IR_ADC_COUNTER
  //this->write_byte(SI1132_PARAM_WR_REG, SI1132_511_ADC_CLOCK); // Set highest integration time (most sensitivity)
  //this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_IR_ADC_COUNTER_PARAM_OFFSET);

  // SET ALS_IR_ADC_GAIN
  this->write_byte(SI1132_PARAM_WR_REG, SI1132_16_DIVIDED_ADC_CLOCK);//SI1132_1_DIVIDED_ADC_CLOCK); // gain 128 is highest
  this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_IR_ADC_GAIN_PARAM_OFFSET);

  // SET ALS_IR_ADC_MISC
  //this->write_byte(SI1132_PARAM_WR_REG, SI1132_HIGH_SIGNAL_RANGE); // Uncomment to set "high signal range" mode for operation under direct sunlight
  //this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_IR_ADC_MISC_PARAM_OFFSET);

  // SET ALS_IR_ADCMUX
  //this->write_byte(SI1132_PARAM_WR_REG, SI1132_ALS_IR_ADCMUX_SMALLIR);
  //this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_IR_ADCMUX_PARAM_OFFSET);


  /* Visible */
  // SET ALS_VIS_ADC_COUNTER
  //this->write_byte(SI1132_PARAM_WR_REG, SI1132_511_ADC_CLOCK); // Set highest integration time (most sensitivity)
  //this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_VIS_ADC_COUNTER_PARAM_OFFSET);

  // SET ALS_VIS_ADC_GAIN
  this->write_byte(SI1132_PARAM_WR_REG, SI1132_32_DIVIDED_ADC_CLOCK);//SI1132_1_DIVIDED_ADC_CLOCK); // gain 128 is highest
  this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_VIS_ADC_GAIN_PARAM_OFFSET);

  // SET ALS_VIS_ADC_MISC
  //this->write_byte(SI1132_PARAM_WR_REG, SI1132_HIGH_SIGNAL_RANGE); // Uncomment to set "high signal range" mode for operation under direct sunlight
  //this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_VIS_ADC_MISC_PARAM_OFFSET);


  // SET AUX_ADCMUX
  //this->write_byte(SI1132_PARAM_WR_REG, SI1132_AUX_ADCMUX_TEMPERATURE);
  //this->write_byte(SI1132_PARAM_WR_REG, SI1132_AUX_ADCMUX_VDD);
  //this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_AUX_ADCMUX_PARAM_OFFSET);

  // COMMAND
  //this->write_byte(SI1132_COMMAND_REG, SI1132_COMMAND_ALS_AUTO);
  //delay(10);
  //});
  
  // Force measurement
  this->write_byte(SI1132_COMMAND_REG, SI1132_COMMAND_ALS_FORCE);
}

void SI1132Component::update(){
  uint8_t buffer[4];
  
  if (!this->read_bytes(SI1132_VISIBLE_DATA_REG, buffer, 4)) {
    ESP_LOGW(TAG, "Error reading registers for si1132.");
    this->status_set_warning();
    return;
  }
  
  if (this->luminosity_sensor_ != nullptr) {
      int32_t luminosity = ((((uint32_t)buffer[1])<<8) | (uint32_t)buffer[0]);
      luminosity -= 256; // the ADC has a 256 offset because "values near 0 can sometimes be negative due to noise" (datasheet)
      if (luminosity <= 0) luminosity = 0;
      luminosity <<= 1; // ADC alignment is set to MSB, so we must include the missing LSB here (<<1)
      this->luminosity_sensor_->publish_state( ((float)luminosity)/(0.282f * 32.0f) ); // 32 is the gain factor, the other factor is lux conversion from datasheet
  }
  if (this->infrared_sensor_ != nullptr) {
      int32_t infrared = ((((uint32_t)buffer[3])<<8) | (uint32_t)buffer[2]);
      infrared -= 256; // the ADC has a 256 offset because "values near 0 can sometimes be negative due to noise" (datasheet)
      if (infrared <= 0) infrared = 0;
      infrared <<= 1; // ADC alignment is set to MSB, so we must include the missing LSB here (<<1)
      this->infrared_sensor_->publish_state( ((float)infrared)/(2.44f * 16.0f) ); // 16 is the gain factor, the other factor is lux conversion from datasheet
  }
  this->status_clear_warning();

      // Reset
      this->write_byte(SI1132_COMMAND_REG, SI1132_COMMAND_RESET);
      delay(1);
    
      // Select HW_KEY register
      this->write_byte(SI1132_HW_KEY_REG, SI1132_HW_KEY_DEFAULT);
      
      // SET PARAM_WR(Chiplist)
      this->write_byte(SI1132_PARAM_WR_REG, SI1132_EN_ALS_IR|SI1132_EN_ALS_VIS); // Add SI1132_EN_UV if you need the (bad) UV estimation
      // COMMAND(Set Chiplist)
      this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_CHIPLIST_PARAM_OFFSET);
    
      
      // SET ALS_IR_ADC_GAIN
      this->write_byte(SI1132_PARAM_WR_REG, SI1132_16_DIVIDED_ADC_CLOCK);//SI1132_1_DIVIDED_ADC_CLOCK); // gain 128 is highest
      this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_IR_ADC_GAIN_PARAM_OFFSET);
      
      // SET ALS_VIS_ADC_GAIN
      this->write_byte(SI1132_PARAM_WR_REG, SI1132_32_DIVIDED_ADC_CLOCK);//SI1132_1_DIVIDED_ADC_CLOCK); // gain 128 is highest
      this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_VIS_ADC_GAIN_PARAM_OFFSET);
      
      // SET PARAM_WR(ALS_ENCODING)
      //this->write_byte(SI1132_PARAM_WR_REG, SI1132_ALS_VIS_ALIGN | SI1132_ALS_IR_ALIGN); // Uncomment to read LSBs from the 13 bit ADC
      //this->write_byte(SI1132_COMMAND_REG, SI1132_PARAM_SET|SI1132_ALS_ENCODING_PARAM_OFFSET);
  
  // Force measurement
  this->write_byte(SI1132_COMMAND_REG, SI1132_COMMAND_ALS_FORCE);
}

void SI1132Component::dump_config(){
  ESP_LOGCONFIG(TAG, "SI1132 component");
}

}  // namespace SI1132Sensor
}  // namespace esphome
