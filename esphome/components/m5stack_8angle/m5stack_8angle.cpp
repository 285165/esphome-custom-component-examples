#include "m5stack_8angle.h"

#include "esphome/core/log.h"

namespace esphome {
namespace m5stack_8angle {

static const char *const TAG = "m5stack_8angle";

void M5Stack8Angle::set_channel_sensor(uint8_t channel,
                                       sensor::Sensor *entity,
                                       uint8_t bit_depth) {
  if (channel >= 8)
    return;
  this->channel_sensors_[channel] = entity;
  this->channel_bit_depths_[channel] = bit_depth;
}

bool M5Stack8Angle::read_regs_(uint8_t reg, uint8_t *data, uint8_t len) {
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

  this->firmware_version_valid_ =
      this->read_regs_(REG_FIRMWARE_VERSION, &this->firmware_version_, 1);

  if (this->change_i2c_address_ && !this->change_device_address_()) {
    ESP_LOGE(TAG, "Failed to change I2C address");
    this->mark_failed();
  }
}

void M5Stack8Angle::update() {
  if (!this->present_ || this->is_failed())
    return;

  bool all_ok = true;
  uint8_t analog_12bit[16];
  uint8_t analog_8bit[8];

  bool need_12bit = false;
  bool need_8bit = false;
  for (uint8_t channel = 0; channel < 8; channel++) {
    if (this->channel_sensors_[channel] == nullptr)
      continue;
    if (this->channel_bit_depths_[channel] == 8)
      need_8bit = true;
    else
      need_12bit = true;
  }

  bool read_12bit_ok = true;
  bool read_8bit_ok = true;

  // Read all configured 12-bit channels in one transaction: 0x00..0x0F.
  if (need_12bit) {
    read_12bit_ok = this->read_regs_(REG_ANALOG_12BIT_BASE,
                                     analog_12bit,
                                     sizeof(analog_12bit));
    if (!read_12bit_ok) {
      ESP_LOGW(TAG, "Failed to read 12-bit channel block 0x00..0x0F");
      all_ok = false;
    }
  }

  // Read all configured 8-bit channels in one transaction: 0x10..0x17.
  if (need_8bit) {
    read_8bit_ok = this->read_regs_(REG_ANALOG_8BIT_BASE,
                                    analog_8bit,
                                    sizeof(analog_8bit));
    if (!read_8bit_ok) {
      ESP_LOGW(TAG, "Failed to read 8-bit channel block 0x10..0x17");
      all_ok = false;
    }
  }

  for (uint8_t channel = 0; channel < 8; channel++) {
    if (this->channel_sensors_[channel] == nullptr)
      continue;

    uint16_t value;
    if (this->channel_bit_depths_[channel] == 8) {
      if (!read_8bit_ok)
        continue;
      value = analog_8bit[channel];
    } else {
      if (!read_12bit_ok)
        continue;
      const uint8_t offset = channel * 2;
      value = (static_cast<uint16_t>(analog_12bit[offset]) |
               (static_cast<uint16_t>(analog_12bit[offset + 1]) << 8)) & 0x0FFF;
    }

    if (!this->last_channel_valid_[channel] ||
        value != this->last_channel_values_[channel]) {
      this->last_channel_values_[channel] = value;
      this->last_channel_valid_[channel] = true;
      this->channel_sensors_[channel]->publish_state(static_cast<float>(value));
    }
  }

  // SW remains at documented register 0x20.
  if (this->sw_sensor_ != nullptr) {
    uint8_t raw = 0;
    if (!this->read_regs_(REG_SW, &raw, 1)) {
      ESP_LOGW(TAG, "Failed to read SW");
      all_ok = false;
    } else {
      const bool value = raw != 0;
      if (!this->last_sw_valid_ || value != this->last_sw_value_) {
        this->last_sw_value_ = value;
        this->last_sw_valid_ = true;
        this->sw_sensor_->publish_state(value);
      }
    }
  }

  if (all_ok)
    this->status_clear_warning();
  else
    this->status_set_warning();
}

bool M5Stack8Angle::set_led(uint8_t index,
                            uint8_t red,
                            uint8_t green,
                            uint8_t blue,
                            uint8_t brightness) {
  if (!this->present_ || this->is_failed() || index > 8)
    return false;
  if (brightness > 100)
    brightness = 100;
  const uint8_t data[5] = {
      this->led_register_(index), red, green, blue, brightness};
  return this->write(data, sizeof(data)) == i2c::ERROR_OK;
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
  this->parent_->set_led(
      this->index_, static_cast<uint8_t>(red * 255.0f),
      static_cast<uint8_t>(green * 255.0f),
      static_cast<uint8_t>(blue * 255.0f), 100);
}

}  // namespace m5stack_8angle
}  // namespace esphome
