#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace m5_timerpwr {

class M5_timerpwr: public PollingComponent, public i2c::I2CDevice {
 public:
  void set_charging(binary_sensor::BinarySensor *charging) { this->charging_ = charging; }

  void set_battery_level(sensor::Sensor *battery_level) { this->battery_level_ = battery_level; }
  void set_battery_voltage(sensor::Sensor *battery_voltage) { this->battery_voltage_ = battery_voltage; }
  void set_battery_current(sensor::Sensor *battery_current) { this->battery_current_ = battery_current; }

  void set_usb_voltage(sensor::Sensor *usb_voltage) { this->usb_voltage_ = usb_voltage; }
  void set_usb_current(sensor::Sensor *usb_current) { this->usb_current_ = usb_current; }

  void set_grove_voltage(sensor::Sensor *grove_voltage) { this->grove_voltage_ = grove_voltage; }
  void set_grove_current(sensor::Sensor *grove_current) { this->grove_current_ = grove_current; }

  void update() override;
  void setup() override;
  void loop() override;
  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  uint8_t version_{0};
  binary_sensor::BinarySensor *charging_{nullptr};

  sensor::Sensor *battery_level_{nullptr};
  sensor::Sensor *battery_voltage_{nullptr};
  sensor::Sensor *battery_current_{nullptr};

  sensor::Sensor *usb_voltage_{nullptr};
  sensor::Sensor *usb_current_{nullptr};

  sensor::Sensor *grove_voltage_{nullptr};
  sensor::Sensor *grove_current_{nullptr};
};

}  // namespace m5_timerpwr
}  // namespace esphome