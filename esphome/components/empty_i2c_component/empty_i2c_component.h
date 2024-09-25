#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/optional.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/i2c/i2c_bus.h"
#include <utility>

namespace esphome {
namespace empty_i2c_component {

class EmptyI2CComponent : public i2c::I2CDevice, public Component {
 public:
  EmptyI2CComponent();
  void setup() override;
  void loop() override;
  void dump_config() override;
  
  /// @brief We store the address of the device on the bus
  /// @param address of the device
  void set_i2c_address(uint8_t address) { this->address_ = address; }

  /// @brief we store the pointer to the I2CBus to use
  /// @param bus pointer to the I2CBus object
  //void set_i2c_bus(I2CBus *bus) { bus_ = bus; }
  
  void set_my_required_key(std::string my_required_key) { this->my_required_key_ = std::move(y_required_key); };
  void set_my_optional_key(int my_required_key) { this->my_required_key_ = my_required_key; }
  
 protected:
  uint8_t address_{0x00};  ///< store the address of the device on the bus
  int my_optional_key_;
  std::string my_required_key_;
  //I2CBus *bus_{nullptr};   ///< pointer to I2CBus instance
};


}  // namespace empty_i2c_component
}  // namespace esphome