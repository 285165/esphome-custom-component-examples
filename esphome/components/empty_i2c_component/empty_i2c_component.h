#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/optional.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/i2c/i2c_bus.h"

namespace esphome {
namespace empty_i2c_component {

class EmptyI2CComponent : public i2c::I2CDevice, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  
  /// @brief We store the address of the device on the bus
  /// @param address of the device
  void set_i2c_address(uint8_t address) { address_ = address; }

  /// @brief we store the pointer to the I2CBus to use
  /// @param bus pointer to the I2CBus object
  //void set_i2c_bus(I2CBus *bus) { bus_ = bus; }
  
 protected:
  uint8_t address_{0x00};  ///< store the address of the device on the bus
  //I2CBus *bus_{nullptr};   ///< pointer to I2CBus instance
};


}  // namespace empty_i2c_component
}  // namespace esphome