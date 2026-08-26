#pragma once

#include <array>
#include <cstdint>

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/button/button.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace m5stack_8encoder {

class M5Stack8Encoder : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_encoder_sensor(uint8_t index, sensor::Sensor *entity);
  void set_increment_sensor(uint8_t index, sensor::Sensor *entity) {
    if (index < this->increment_sensors_.size())
      this->increment_sensors_[index] = entity;
  }
  void set_button_sensor(uint8_t index, binary_sensor::BinarySensor *entity,
                         uint8_t pressed_value);
  void set_sw_sensor(binary_sensor::BinarySensor *entity) { this->sw_sensor_ = entity; }
  void set_change_i2c_address_to(uint8_t address) {
    this->change_i2c_address_ = true;
    this->new_i2c_address_ = address;
  }

  bool set_led(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
  bool reset_all_counters();

 protected:
  static constexpr uint8_t REG_COUNTER_BASE = 0x00;
  static constexpr uint8_t REG_INCREMENT_BASE = 0x20;
  static constexpr uint8_t REG_RESET_BASE = 0x40;
  static constexpr uint8_t REG_BUTTON_BASE = 0x50;
  static constexpr uint8_t REG_SW = 0x60;
  static constexpr uint8_t REG_RGB_BASE = 0x70;
  static constexpr uint8_t REG_FIRMWARE_VERSION = 0xFE;
  static constexpr uint8_t REG_I2C_ADDRESS = 0xFF;

  bool read_regs_(uint8_t reg, uint8_t *data, uint8_t len);
  bool change_device_address_();

  bool present_{false};
  bool change_i2c_address_{false};
  uint8_t new_i2c_address_{0x41};
  std::array<sensor::Sensor *, 8> encoder_sensors_{};
  std::array<sensor::Sensor *, 8> increment_sensors_{};
  std::array<binary_sensor::BinarySensor *, 8> button_sensors_{};
  std::array<uint8_t, 8> button_pressed_values_{};
  binary_sensor::BinarySensor *sw_sensor_{nullptr};
  uint8_t firmware_version_{0};
  bool firmware_version_valid_{false};
};

class M5Stack8EncoderResetAllButton : public button::Button {
 public:
  void set_parent(M5Stack8Encoder *parent) { this->parent_ = parent; }
 protected:
  void press_action() override;
  M5Stack8Encoder *parent_{nullptr};
};

class M5Stack8EncoderLight : public light::LightOutput {
 public:
  M5Stack8EncoderLight(M5Stack8Encoder *parent, uint8_t index)
      : parent_(parent), index_(index) {}
  light::LightTraits get_traits() override;
  void write_state(light::LightState *state) override;
 protected:
  M5Stack8Encoder *parent_;
  uint8_t index_;
};

}  // namespace m5stack_8encoder
}  // namespace esphome
