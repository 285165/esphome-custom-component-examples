#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/optional.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/i2c/i2c_bus.h"
#include <utility>

namespace esphome {
namespace empty_i2c_component {

class EmptyI2CComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  EmptyI2CComponent();

  void setup() override;
  void loop() override;
  void update() override;
  void dump_config() override;
  
  /// @brief We store the address of the device on the bus
  /// @param address of the device
  void set_i2c_address(uint8_t address) { this->address_ = address; }

  /// @brief we store the pointer to the I2CBus to use
  /// @param bus pointer to the I2CBus object
  //void set_i2c_bus(I2CBus *bus) { bus_ = bus; }
  
  //void set_my_required_key(std::string my_required_key) { this->my_required_key_ = std::move(my_required_key); };
  void set_my_required_key(const std::string &my_required_key) { this->my_required_key_ = my_required_key; };
  void set_my_optional_key(int my_optional_key) { this->my_optional_key_ = my_optional_key; }

  bool available() {return true;}
  void set_my_altitude_sensor(sensor::Sensor *altitude_sensor) { altitude_sensor_ = altitude_sensor; }
  void set_my_satellites_sensor(sensor::Sensor *satellites_sensor) { satellites_sensor_ = satellites_sensor; }
  
 protected:
  uint8_t address_{0x00};  ///< store the address of the device on the bus
  int my_optional_key_;
  std::string my_required_key_;


  bool has_time_{true};
  float altitude_ = -1;
  int satellites_ = -1;

  sensor::Sensor *altitude_sensor_{nullptr};
  sensor::Sensor *satellites_sensor_{nullptr};
};


}  // namespace empty_i2c_component
}  // namespace esphome