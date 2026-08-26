#include "m5stack_u153.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5stack_u153 {

static const char *const TAG = "m5stack_u153";

bool M5StackU153::read_bytes_(uint8_t reg, uint8_t *data, size_t len) {
  const auto error = this->read_register(reg, data, len);
  if (error != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "I2C read failed at 0x%02X, error=%d", reg, error);
    this->status_set_warning();
    return false;
  }
  return true;
}

bool M5StackU153::write_bytes_(uint8_t reg, const uint8_t *data, size_t len) {
  const auto error = this->write_register(reg, data, len);
  if (error != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "I2C write failed at 0x%02X, error=%d", reg, error);
    this->status_set_warning();
    return false;
  }
  return true;
}

void M5StackU153::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Stack U153 Unit 8Encoder...");
  if (!this->read_bytes_(REG_FW_VERSION, &this->firmware_version_, 1)) {
    this->mark_failed();
    return;
  }
  this->status_clear_warning();
}

void M5StackU153::update() {
  bool ok = true;
  for (uint8_t i = 0; i < 8; i++) {
    if (this->encoders_[i] == nullptr) continue;
    uint8_t data[4];
    if (!this->read_bytes_(REG_ENCODER + i * 4, data, sizeof(data))) {
      ok = false;
      continue;
    }
    const uint32_t raw = uint32_t(data[0]) | (uint32_t(data[1]) << 8) |
                         (uint32_t(data[2]) << 16) | (uint32_t(data[3]) << 24);
    this->encoders_[i]->publish_state(static_cast<float>(static_cast<int32_t>(raw)));
  }

  uint8_t buttons[8];
  if (this->read_bytes_(REG_BUTTON, buttons, sizeof(buttons))) {
    for (uint8_t i = 0; i < 8; i++) {
      if (this->buttons_[i] == nullptr) continue;
      bool state = buttons[i] != 0;
      if (this->button_inverted_[i]) state = !state;
      this->buttons_[i]->publish_state(state);
    }
  } else {
    ok = false;
  }

  if (this->toggle_ != nullptr) {
    uint8_t value = 0;
    if (this->read_bytes_(REG_TOGGLE, &value, 1)) {
      bool state = value != 0;
      if (this->toggle_inverted_) state = !state;
      this->toggle_->publish_state(state);
    } else {
      ok = false;
    }
  }
  if (ok) this->status_clear_warning();
}

bool M5StackU153::set_led(uint8_t index, uint8_t red, uint8_t green, uint8_t blue) {
  if (index > 8) return false;
  const uint8_t data[3] = {red, green, blue};
  return this->write_bytes_(REG_LED + index * 3, data, sizeof(data));
}

void M5StackU153::dump_config() {
  ESP_LOGCONFIG(TAG, "M5Stack U153 Unit 8Encoder:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  ESP_LOGCONFIG(TAG, "  Firmware version: 0x%02X", this->firmware_version_);
}

light::LightTraits M5StackU153Light::get_traits() {
  light::LightTraits traits;
  traits.set_supported_color_modes({light::ColorMode::RGB});
  return traits;
}

void M5StackU153Light::write_state(light::LightState *state) {
  float red, green, blue;
  state->current_values_as_rgb(&red, &green, &blue);
  this->parent_->set_led(this->index_, uint8_t(red * 255.0f), uint8_t(green * 255.0f), uint8_t(blue * 255.0f));
}

}  // namespace m5stack_u153
}  // namespace esphome
