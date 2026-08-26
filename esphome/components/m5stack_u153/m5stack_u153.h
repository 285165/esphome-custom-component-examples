#pragma once

#include <array>
#include <cstdint>
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"

namespace esphome {
namespace m5stack_u153 {

static constexpr uint8_t REG_ENCODER = 0x00;
static constexpr uint8_t REG_BUTTON = 0x20;
static constexpr uint8_t REG_TOGGLE = 0x30;
static constexpr uint8_t REG_LED = 0x40;
static constexpr uint8_t REG_FW_VERSION = 0xFE;

class M5StackU153 : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_encoder_sensor(uint8_t index, sensor::Sensor *value) { encoders_[index] = value; }
  void set_button_sensor(uint8_t index, binary_sensor::BinarySensor *value, bool inverted) {
    buttons_[index] = value;
    button_inverted_[index] = inverted;
  }
  void set_toggle_sensor(binary_sensor::BinarySensor *value, bool inverted) {
    toggle_ = value;
    toggle_inverted_ = inverted;
  }
  bool set_led(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);

 protected:
  bool read_bytes_(uint8_t reg, uint8_t *data, size_t len);
  bool write_bytes_(uint8_t reg, const uint8_t *data, size_t len);
  std::array<sensor::Sensor *, 8> encoders_{};
  std::array<binary_sensor::BinarySensor *, 8> buttons_{};
  std::array<bool, 8> button_inverted_{{true, true, true, true, true, true, true, true}};
  binary_sensor::BinarySensor *toggle_{nullptr};
  bool toggle_inverted_{false};
  uint8_t firmware_version_{0};
};

class M5StackU153Light : public light::LightOutput {
 public:
  M5StackU153Light(M5StackU153 *parent, uint8_t index) : parent_(parent), index_(index) {}
  light::LightTraits get_traits() override;
  void write_state(light::LightState *state) override;
 protected:
  M5StackU153 *parent_;
  uint8_t index_;
};

}  // namespace m5stack_u153
}  // namespace esphome
