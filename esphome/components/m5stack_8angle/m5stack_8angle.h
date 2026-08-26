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
namespace m5stack_8angle {

class M5Stack8Angle : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_channel_sensor(uint8_t channel, sensor::Sensor *entity, uint8_t bit_depth);
  void set_sw_sensor(binary_sensor::BinarySensor *entity) { sw_sensor_ = entity; }
  void set_change_i2c_address_to(uint8_t address) {
    change_i2c_address_ = true;
    new_i2c_address_ = address;
  }
  bool set_led(uint8_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);

 protected:
  static constexpr uint8_t REG_ANALOG_12BIT_BASE = 0x00;
  static constexpr uint8_t REG_ANALOG_8BIT_BASE = 0x10;
  static constexpr uint8_t REG_SW = 0x20;
  static constexpr uint8_t REG_RGB_LED0_BASE = 0x30;
  static constexpr uint8_t REG_RGB_LED4_BASE = 0x40;
  static constexpr uint8_t REG_RGB_LED8_BASE = 0x50;
  static constexpr uint8_t REG_FIRMWARE_VERSION = 0xF0;
  static constexpr uint8_t REG_I2C_ADDRESS = 0xFF;

  bool read_regs_(uint8_t reg, uint8_t *data, uint8_t len);
  bool change_device_address_();
  uint8_t led_register_(uint8_t index) const;

  bool present_{false};
  bool change_i2c_address_{false};
  uint8_t new_i2c_address_{0x43};

  std::array<sensor::Sensor *, 8> channel_sensors_{};
  std::array<uint8_t, 8> channel_bit_depths_{{12, 12, 12, 12, 12, 12, 12, 12}};
  std::array<uint16_t, 8> last_channel_values_{};
  std::array<bool, 8> last_channel_valid_{};

  binary_sensor::BinarySensor *sw_sensor_{nullptr};
  bool last_sw_value_{false};
  bool last_sw_valid_{false};

  uint8_t firmware_version_{0};
  bool firmware_version_valid_{false};
};

class M5Stack8AngleLight : public light::LightOutput {
 public:
  M5Stack8AngleLight(M5Stack8Angle *parent, uint8_t index) : parent_(parent), index_(index) {}
  light::LightTraits get_traits() override;
  void write_state(light::LightState *state) override;
 protected:
  M5Stack8Angle *parent_;
  uint8_t index_;
};

}  // namespace m5stack_8angle
}  // namespace esphome
