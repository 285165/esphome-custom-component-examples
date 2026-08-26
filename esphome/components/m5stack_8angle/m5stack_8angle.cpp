#include "m5stack_8angle.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5stack_8angle {

static const char *const TAG = "m5stack_8angle";

void M5Stack8Angle::set_channel_sensor(uint8_t channel, sensor::Sensor *entity, uint8_t bit_depth) {
  if (channel >= 8) return;
  channel_sensors_[channel] = entity;
  channel_bit_depths_[channel] = bit_depth;
}

bool M5Stack8Angle::read_regs_(uint8_t reg, uint8_t *data, uint8_t len) {
  if (write(&reg, 1) != i2c::ERROR_OK) return false;
  return read(data, len) == i2c::ERROR_OK;
}

uint8_t M5Stack8Angle::led_register_(uint8_t index) const {
  if (index < 4) return REG_RGB_LED0_BASE + index * 4;
  if (index < 8) return REG_RGB_LED4_BASE + (index - 4) * 4;
  return REG_RGB_LED8_BASE;
}

bool M5Stack8Angle::change_device_address_() {
  const uint8_t data[2] = {REG_I2C_ADDRESS, new_i2c_address_};
  if (write(data, sizeof(data)) != i2c::ERROR_OK) return false;
  ESP_LOGW(TAG, "I2C address changed from 0x%02X to 0x%02X", address_, new_i2c_address_);
  set_i2c_address(new_i2c_address_);
  return true;
}

void M5Stack8Angle::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Stack Unit 8Angle...");
  uint8_t reg = REG_ANALOG_12BIT_BASE;
  if (write(&reg, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Unit 8Angle not found at address 0x%02X", address_);
    mark_failed();
    return;
  }
  present_ = true;
  firmware_version_valid_ = read_regs_(REG_FIRMWARE_VERSION, &firmware_version_, 1);
  if (change_i2c_address_ && !change_device_address_()) {
    ESP_LOGE(TAG, "Failed to change I2C address");
    mark_failed();
  }
}

void M5Stack8Angle::update() {
  if (!present_ || is_failed()) return;
  bool all_ok = true;

  for (uint8_t channel = 0; channel < 8; channel++) {
    if (channel_sensors_[channel] == nullptr) continue;

    uint16_t value = 0;
    if (channel_bit_depths_[channel] == 8) {
      uint8_t raw = 0;
      if (!read_regs_(REG_ANALOG_8BIT_BASE + channel, &raw, 1)) {
        ESP_LOGW(TAG, "Failed to read CH%u 8-bit value", channel + 1);
        all_ok = false;
        continue;
      }
      value = raw;
    } else {
      uint8_t data[2];
      if (!read_regs_(REG_ANALOG_12BIT_BASE + channel * 2, data, 2)) {
        ESP_LOGW(TAG, "Failed to read CH%u 12-bit value", channel + 1);
        all_ok = false;
        continue;
      }
      value = (uint16_t(data[0]) | (uint16_t(data[1]) << 8)) & 0x0FFF;
    }

    if (!last_channel_valid_[channel] || value != last_channel_values_[channel]) {
      last_channel_values_[channel] = value;
      last_channel_valid_[channel] = true;
      channel_sensors_[channel]->publish_state(static_cast<float>(value));
    }
  }

  if (sw_sensor_ != nullptr) {
    uint8_t raw = 0;
    if (!read_regs_(REG_SW, &raw, 1)) {
      ESP_LOGW(TAG, "Failed to read SW");
      all_ok = false;
    } else {
      const bool value = raw != 0;
      if (!last_sw_valid_ || value != last_sw_value_) {
        last_sw_value_ = value;
        last_sw_valid_ = true;
        sw_sensor_->publish_state(value);
      }
    }
  }

  if (all_ok) status_clear_warning(); else status_set_warning();
}

bool M5Stack8Angle::set_led(uint8_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness) {
  if (!present_ || is_failed() || index > 8) return false;
  if (brightness > 100) brightness = 100;
  const uint8_t data[5] = {led_register_(index), red, green, blue, brightness};
  return write(data, sizeof(data)) == i2c::ERROR_OK;
}

void M5Stack8Angle::dump_config() {
  ESP_LOGCONFIG(TAG, "M5Stack Unit 8Angle:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  if (firmware_version_valid_) ESP_LOGCONFIG(TAG, "  Firmware version: 0x%02X", firmware_version_);
  else ESP_LOGCONFIG(TAG, "  Firmware version: unavailable");
}

light::LightTraits M5Stack8AngleLight::get_traits() {
  light::LightTraits traits;
  traits.set_supported_color_modes({light::ColorMode::RGB});
  return traits;
}
void M5Stack8AngleLight::write_state(light::LightState *state) {
  float red, green, blue;
  state->current_values_as_rgb(&red, &green, &blue);
  parent_->set_led(index_, uint8_t(red * 255.0f), uint8_t(green * 255.0f), uint8_t(blue * 255.0f), 100);
}

}  // namespace m5stack_8angle
}  // namespace esphome
