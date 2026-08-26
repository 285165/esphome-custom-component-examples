#pragma once

#include <array>
#include <cstdint>

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace m5stack_u153 {

class M5StackU153 : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_encoder_sensor(uint8_t index, sensor::Sensor *entity) {
    if (index < this->encoder_sensors_.size())
      this->encoder_sensors_[index] = entity;
  }

  void set_button_sensor(uint8_t index,
                         binary_sensor::BinarySensor *entity,
                         uint8_t pressed_value) {
    if (index < this->button_sensors_.size()) {
      this->button_sensors_[index] = entity;
      this->button_pressed_values_[index] = pressed_value;
    }
  }

  void set_switch_sensor(binary_sensor::BinarySensor *entity) {
    this->switch_sensor_ = entity;
  }

  bool set_led(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);

 protected:
  static constexpr uint8_t REG_COUNTER_BASE = 0x00;
  static constexpr uint8_t REG_BUTTON_BASE = 0x50;
  static constexpr uint8_t REG_SWITCH = 0x60;
  static constexpr uint8_t REG_RGB_BASE = 0x70;
  static constexpr uint8_t REG_FIRMWARE_VERSION = 0xF0;

  // U153 requires write(register)+STOP followed by a separate read transaction.
  bool read_regs_(uint8_t reg, uint8_t *data, uint8_t len);

  bool present_{false};
  std::array<sensor::Sensor *, 8> encoder_sensors_{};
  std::array<binary_sensor::BinarySensor *, 8> button_sensors_{};
  std::array<uint8_t, 8> button_pressed_values_{};
  binary_sensor::BinarySensor *switch_sensor_{nullptr};
  uint8_t firmware_version_{0};
  bool firmware_version_valid_{false};
};

class M5StackU153Light : public light::LightOutput {
 public:
  M5StackU153Light(M5StackU153 *parent, uint8_t index)
      : parent_(parent), index_(index) {}

  light::LightTraits get_traits() override;
  void write_state(light::LightState *state) override;

 protected:
  M5StackU153 *parent_;
  uint8_t index_;
};

}  // namespace m5stack_u153
}  // namespace esphome
