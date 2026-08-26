#include "m5stack_8angle.h"

#include "esphome/core/log.h"

namespace esphome {
namespace m5stack_8angle {

static const char *const TAG = "m5stack_8angle";

void M5Stack8Angle::set_channel_sensor(uint8_t channel,
                                       sensor::Sensor *entity,
                                       uint8_t bit_depth) {
  if (channel >= this->channel_sensors_.size())
    return;
  this->channel_sensors_[channel] = entity;
  this->channel_bit_depths_[channel] = bit_depth;
}

bool M5Stack8Angle::read_regs_(uint8_t reg, uint8_t *data, uint8_t len) {
  // Write register pointer with STOP, then perform a separate read transaction.
  if (this->write(&reg, 1) != i2c::ERROR_OK)
    return false;
  return this->read(data, len) == i2c::ERROR_OK;
}

uint8_t M5Stack8Angle::led_register_(uint8_t index) const {
  if (index < 4)
    return REG_RGB_LED0_BASE + index * 4;
  if (index < 8)
    return REG_RGB_LED4_BASE + (index - 4) * 4;
  return REG_RGB_LED8_BASE;
}

bool M5Stack8Angle::change_device_address_() {
  const uint8_t data[2] = {REG_I2C_ADDRESS, this->new_i2c_address_};
  if (this->write(data, sizeof(data)) != i2c::ERROR_OK)
    return false;

  ESP_LOGW(TAG, "I2C address changed from 0x%02X to 0x%02X",
           this->address_, this->new_i2c_address_);
  this->set_i2c_address(this->new_i2c_address_);
  return true;
}

void M5Stack8Angle::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Stack Unit 8Angle...");

  uint8_t reg = REG_ANALOG_12BIT_BASE;
  if (this->write(&reg, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Unit 8Angle not found at address 0x%02X", this->address_);
    this->mark_failed();
    return;
  }
  this->present_ = true;

  uint8_t version = 0;
  this->firmware_version_valid_ =
      this->read_regs_(REG_FIRMWARE_VERSION, &version, 1);
  if (this->firmware_version_valid_)
    this->firmware_version_ = version;

  if (this->change_i2c_address_ && !this->change_device_address_()) {
    ESP_LOGE(TAG, "Failed to change I2C address");
    this->mark_failed();
  }
}

void M5Stack8Angle::update() {
  if (!this->present_ || this->is_failed())
    return;

  bool all_ok = true;

  for (uint8_t channel = 0; channel < 8; channel++) {
    if (this->channel_sensors_[channel] == nullptr)
      continue;

    if (this->channel_bit_depths_[channel] == 8) {
      uint8_t value = 0;
      const uint8_t reg = REG_ANALOG_8BIT_BASE + channel;
      if (!this->read_regs_(reg, &value, 1)) {
        ESP_LOGW(TAG, "Failed to read CH%u 8-bit value", channel + 1);
        all_ok = false;
        continue;
      }
      this->channel_sensors_[channel]->publish_state(value);
    } else {
      uint8_t data[2];
      const uint8_t reg = REG_ANALOG_12BIT_BASE + channel * 2;
      if (!this->read_regs_(reg, data, sizeof(data))) {
        ESP_LOGW(TAG, "Failed to read CH%u 12-bit value", channel + 1);
        all_ok = false;
        continue;
      }
      const uint16_t value =
          (static_cast<uint16_t>(data[0]) |
           (static_cast<uint16_t>(data[1]) << 8)) & 0x0FFF;
      this->channel_sensors_[channel]->publish_state(value);
    }
  }

  if (this->sw_sensor_ != nullptr) {
    uint8_t value = 0;
    if (!this->read_regs_(REG_SW, &value, 1)) {
      ESP_LOGW(TAG, "Failed to read SW");
      all_ok = false;
    } else {
      this->sw_sensor_->publish_state(value != 0);
    }
  }

  if (all_ok)
    this->status_clear_warning();
  else
    this->status_set_warning();
}

bool M5Stack8Angle::set_led(uint8_t index, uint8_t red, uint8_t green,
                            uint8_t blue, uint8_t brightness) {
  if (!this->present_ || this->is_failed() || index > 8)
    return false;

  if (brightness > 100)
    brightness = 100;

  const uint8_t data[5] = {
      this->led_register_(index), red, green, blue, brightness};
  if (this->write(data, sizeof(data)) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to write LED %u", index);
    return false;
  }
  return true;
}

void M5Stack8Angle::dump_config() {
  ESP_LOGCONFIG(TAG, "M5Stack Unit 8Angle:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  if (this->firmware_version_valid_)
    ESP_LOGCONFIG(TAG, "  Firmware version: 0x%02X", this->firmware_version_);
  else
    ESP_LOGCONFIG(TAG, "  Firmware version: unavailable");
}

light::LightTraits M5Stack8AngleLight::get_traits() {
  light::LightTraits traits;
  traits.set_supported_color_modes({light::ColorMode::RGB});
  return traits;
}

void M5Stack8AngleLight::write_state(light::LightState *state) {
  float red;
  float green;
  float blue;
  state->current_values_as_rgb(&red, &green, &blue);

  // current_values_as_rgb includes the ESPHome brightness scaling.
  // Hardware brightness is kept at 100%; RGB values carry the effective level.
  this->parent_->set_led(
      this->index_, static_cast<uint8_t>(red * 255.0f),
      static_cast<uint8_t>(green * 255.0f),
      static_cast<uint8_t>(blue * 255.0f), 100);
}

}  // namespace m5stack_8angle
}  // namespace esphome
