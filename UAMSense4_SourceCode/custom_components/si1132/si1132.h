
#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace si1132 {

class SI1132Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_luminosity_sensor(sensor::Sensor *luminosity_sensor) { luminosity_sensor_ = luminosity_sensor; }
  void set_infrared_sensor(sensor::Sensor *infrared_sensor) { infrared_sensor_ = infrared_sensor; }
  
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override;
  
 protected:
  sensor::Sensor *luminosity_sensor_;
  sensor::Sensor *infrared_sensor_;
  int iter_{0};
};

}  // namespace SI1132
}  // namespace esphome
