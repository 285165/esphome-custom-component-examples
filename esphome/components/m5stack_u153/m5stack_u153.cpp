#include "m5stack_u153.h"

#include "esphome/core/log.h"

namespace esphome {
namespace m5stack_u153 {

static const char *const TAG = "m5stack_u153";

void M5StackU153::set_encoder_sensor(uint8_t index, sensor::Sensor *entity) {
  if (index < this->encoder_sensors_.size())
    this->encoder_sensors_[index] = entity;
}

void M5StackU153::set_button_sensor(
    uint8_t index, binary_sensor::BinarySensor *entity,
    uint8_t pressed_value) {
  if (index < this->button_sensors_.size()) {
    this->button_sensors_[index] = entity;
    this->button_pressed_values_[index] = pressed_value;
  }
}

bool M5StackU153::read_register_bytes_(uint8_t reg, uint8_t *data,
                                       size_t len, bool warn) {
  const auto error = this->read_register(reg, data, len);
  if (error == i2c::ERROR_OK)
    return true;

  if (warn)
    ESP_LOGW(TAG, "I2C read failed: device=0x%02X register=0x%02X error=%d",
             this->address_, reg, error);
  return false;
}

bool M5StackU153::write_register_bytes_(uint8_t reg, const uint8_t *data,
                                        size_t len) {
  const auto error = this->write_register(reg, data, len);
  if (error == i2c::ERROR_OK)
    return true;

  ESP_LOGW(TAG, "I2C write failed: device=0x%02X register=0x%02X error=%d",
           this->address_, reg, error);
  this->status_set_warning();
  return false;
}

void M5StackU153::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Stack U153 Unit 8Encoder...");

  // Firmware version resides at 0xFE according to the V1 register map.
  // Failure of this optional diagnostic read must not disable the device.
  this->firmware_version_valid_ = this->read_register_bytes_(
      REG_FIRMWARE_VERSION, &this->firmware_version_, 1, false);
}

void M5StackU153::update() {
  bool ok = true;

  // Counter values: 8 signed 32-bit little-endian values at 0x00..0x1F.
  uint8_t counters[32];
  if (this->read_register_bytes_(REG_COUNTER_BASE, counters,
                                 sizeof(counters))) {
    for (uint8_t index = 0; index < 8; index++) {
      if (this->encoder_sensors_[index] == nullptr)
        continue;
      const uint8_t *p = &counters[index * 4];
      const uint32_t raw = static_cast<uint32_t>(p[0]) |
                           (static_cast<uint32_t>(p[1]) << 8) |
                           (static_cast<uint32_t>(p[2]) << 16) |
                           (static_cast<uint32_t>(p[3]) << 24);
      this->encoder_sensors_[index]->publish_state(
          static_cast<float>(static_cast<int32_t>(raw)));
    }
  } else {
    ok = false;
  }

  // Button values: BTN0..BTN7 at 0x50..0x57.
  uint8_t buttons[8];
  if (this->read_register_bytes_(REG_BUTTON_BASE, buttons,
                                 sizeof(buttons))) {
    for (uint8_t index = 0; index < 8; index++) {
      if (this->button_sensors_[index] != nullptr) {
        this->button_sensors_[index]->publish_state(
            buttons[index] == this->button_pressed_values_[index]);
      }
    }
  } else {
    ok = false;
  }

  // Physical toggle switch value at 0x60.
  if (this->switch_sensor_ != nullptr) {
    uint8_t value = 0;
    if (this->read_register_bytes_(REG_SWITCH, &value, 1))
      this->switch_sensor_->publish_state(value != 0);
    else
      ok = false;
  }

  if (ok)
    this->status_clear_warning();
  else
    this->status_set_warning();
}

bool M5StackU153::set_led(uint8_t index, uint8_t red, uint8_t green,
                          uint8_t blue) {
  if (index >= 8) {
    ESP_LOGE(TAG, "Invalid LED index: %u", index);
    return false;
  }

  const uint8_t rgb[3] = {red, green, blue};
  return this->write_register_bytes_(REG_RGB_BASE + index * 3, rgb,
                                     sizeof(rgb));
}

void M5StackU153::dump_config() {
  ESP_LOGCONFIG(TAG, "M5Stack U153 Unit 8Encoder:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  if (this->firmware_version_valid_)
    ESP_LOGCONFIG(TAG, "  Firmware version: 0x%02X",
                  this->firmware_version_);
  else
    ESP_LOGCONFIG(TAG, "  Firmware version: unavailable");
}

light::LightTraits M5StackU153Light::get_traits() {
  light::LightTraits traits;
  traits.set_supported_color_modes({light::ColorMode::RGB});
  return traits;
}

void M5StackU153Light::write_state(light::LightState *state) {
  float red;
  float green;
  float blue;
  state->current_values_as_rgb(&red, &green, &blue);
  this->parent_->set_led(
      this->index_, static_cast<uint8_t>(red * 255.0f),
      static_cast<uint8_t>(green * 255.0f),
      static_cast<uint8_t>(blue * 255.0f));
}

}  // namespace m5stack_u153
}  // namespace esphome
