#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/gpio_expander/cached_gpio.h"

namespace esphome {
namespace m5_4in8out {

// PCF8574(8 pins)/PCF8575(16 pins) always read/write all pins in a single I2C transaction
// so we use uint16_t as bank type to ensure all pins are in one bank and cached together
class M5Stack4In8OutComponent : public Component,
                         public i2c::I2CDevice,
                         public gpio_expander::CachedGpioExpander<uint16_t, 16> {
 public:
  M5Stack4In8OutComponent() = default;

  /// Check i2c availability and setup masks
  void setup() override;
  /// Invalidate cache at start of each loop
  void loop() override;
  /// Helper function to set the pin mode of a pin.
  void pin_mode(uint8_t pin, gpio::Flags flags);

  float get_setup_priority() const override;
  float get_loop_priority() const override;

  void dump_config() override;

 protected:
  bool digital_read_hw(uint8_t pin) override;
  bool digital_read_cache(uint8_t pin) override;
  void digital_write_hw(uint8_t pin, bool value) override;

  bool read_gpio_();
  bool write_gpio_();

  /// Mask for the pin mode - 1 means output, 0 means input
  uint16_t mode_mask_{0x00};
  /// The mask to write as output state - 1 means HIGH, 0 means LOW
  uint16_t output_mask_{0x00};
  /// The state read in read_gpio_ - 1 means HIGH, 0 means LOW
  uint16_t input_mask_{0x00};
};

/// Helper class to expose a PCF8574 pin as an internal input GPIO pin.
class M5Stack4In8OutGPIOPin : public GPIOPin {
 public:
  void setup() override;
  void pin_mode(gpio::Flags flags) override;
  bool digital_read() override;
  void digital_write(bool value) override;
  std::string dump_summary() const override;

  void set_parent(M5Stack4In8OutComponent *parent) { parent_ = parent; }
  void set_pin(uint8_t pin) { pin_ = pin; }
  void set_inverted(bool inverted) { inverted_ = inverted; }
  void set_flags(gpio::Flags flags) { flags_ = flags; }

  gpio::Flags get_flags() const override { return this->flags_; }

 protected:
  M5Stack4In8OutComponent *parent_;
  uint8_t pin_;
  bool inverted_;
  gpio::Flags flags_;
};

}  // namespace m5_4in8out
}  // namespace esphome