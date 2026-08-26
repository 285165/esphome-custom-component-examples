#include "m5stack_8encoder.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5stack_8encoder {

static const char *const TAG = "m5stack_8encoder";

void M5Stack8Encoder::set_encoder_sensor(uint8_t index, sensor::Sensor *entity) {
  if (index < 8) encoder_sensors_[index] = entity;
}
void M5Stack8Encoder::set_increment_sensor(uint8_t index, sensor::Sensor *entity) {
  if (index < 8) increment_sensors_[index] = entity;
}
void M5Stack8Encoder::set_button_sensor(uint8_t index, binary_sensor::BinarySensor *entity, uint8_t pressed_value) {
  if (index < 8) { button_sensors_[index] = entity; button_pressed_values_[index] = pressed_value; }
}

bool M5Stack8Encoder::read_regs_(uint8_t reg, uint8_t *data, uint8_t len) {
  if (write(&reg, 1) != i2c::ERROR_OK) return false;
  return read(data, len) == i2c::ERROR_OK;
}

bool M5Stack8Encoder::change_device_address_() {
  const uint8_t data[2] = {REG_I2C_ADDRESS, new_i2c_address_};
  if (write(data, sizeof(data)) != i2c::ERROR_OK) return false;
  ESP_LOGW(TAG, "I2C address changed from 0x%02X to 0x%02X", address_, new_i2c_address_);
  set_i2c_address(new_i2c_address_);
  return true;
}

void M5Stack8Encoder::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Stack Unit 8Encoder...");
  uint8_t reg = REG_COUNTER_BASE;
  if (write(&reg, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Unit 8Encoder not found at address 0x%02X", address_);
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

bool M5Stack8Encoder::reset_all_counters() {
  if (!present_ || is_failed()) return false;
  bool all_ok = true;
  for (uint8_t channel = 0; channel < 8; channel++) {
    const uint8_t reg = REG_RESET_BASE + channel;
    if (!write_byte(reg, 1)) {
      ESP_LOGE(TAG, "Failed to reset CH%u using register 0x%02X", channel + 1, reg);
      all_ok = false;
    }
  }
  if (!all_ok) { status_set_warning(); return false; }

  for (uint8_t channel = 0; channel < 8; channel++) {
    last_counter_values_[channel] = 0;
    last_counter_valid_[channel] = true;
    if (encoder_sensors_[channel] != nullptr) encoder_sensors_[channel]->publish_state(0.0f);
  }
  status_clear_warning();
  ESP_LOGI(TAG, "All CH1..CH8 counters reset");
  return true;
}

void M5Stack8Encoder::update() {
  if (!present_ || is_failed()) return;
  bool all_ok = true;

  for (uint8_t channel = 0; channel < 8; channel++) {
    if (encoder_sensors_[channel] == nullptr) continue;
    uint8_t data[4];
    if (!read_regs_(REG_COUNTER_BASE + channel * 4, data, 4)) {
      ESP_LOGW(TAG, "Failed to read CH%u counter", channel + 1); all_ok = false; continue;
    }
    const uint32_t raw = uint32_t(data[0]) | (uint32_t(data[1]) << 8) |
                         (uint32_t(data[2]) << 16) | (uint32_t(data[3]) << 24);
    const int32_t value = static_cast<int32_t>(raw);
    if (!last_counter_valid_[channel] || value != last_counter_values_[channel]) {
      last_counter_values_[channel] = value;
      last_counter_valid_[channel] = true;
      encoder_sensors_[channel]->publish_state(static_cast<float>(value));
    }
  }

  for (uint8_t channel = 0; channel < 8; channel++) {
    if (increment_sensors_[channel] == nullptr) continue;
    uint8_t data[4];
    if (!read_regs_(REG_INCREMENT_BASE + channel * 4, data, 4)) {
      ESP_LOGW(TAG, "Failed to read CH%u increment", channel + 1); all_ok = false; continue;
    }
    const uint32_t raw = uint32_t(data[0]) | (uint32_t(data[1]) << 8) |
                         (uint32_t(data[2]) << 16) | (uint32_t(data[3]) << 24);
    const int32_t value = static_cast<int32_t>(raw);
    if (!last_increment_valid_[channel] || value != last_increment_values_[channel]) {
      last_increment_values_[channel] = value;
      last_increment_valid_[channel] = true;
      increment_sensors_[channel]->publish_state(static_cast<float>(value));
    }
  }

  for (uint8_t channel = 0; channel < 8; channel++) {
    if (button_sensors_[channel] == nullptr) continue;
    uint8_t raw = 0;
    if (!read_regs_(REG_BUTTON_BASE + channel, &raw, 1)) {
      ESP_LOGW(TAG, "Failed to read CH%u button", channel + 1); all_ok = false; continue;
    }
    const bool pressed = raw == button_pressed_values_[channel];
    if (!last_button_valid_[channel] || pressed != last_button_values_[channel]) {
      last_button_values_[channel] = pressed;
      last_button_valid_[channel] = true;
      button_sensors_[channel]->publish_state(pressed);
    }
  }

  if (sw_sensor_ != nullptr) {
    uint8_t raw = 0;
    if (!read_regs_(REG_SW, &raw, 1)) { ESP_LOGW(TAG, "Failed to read SW"); all_ok = false; }
    else {
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

bool M5Stack8Encoder::set_led(uint8_t index, uint8_t red, uint8_t green, uint8_t blue) {
  if (!present_ || is_failed() || index >= 9) return false;
  const uint8_t data[4] = {static_cast<uint8_t>(REG_RGB_BASE + index * 3), red, green, blue};
  return write(data, sizeof(data)) == i2c::ERROR_OK;
}

void M5Stack8Encoder::dump_config() {
  ESP_LOGCONFIG(TAG, "M5Stack Unit 8Encoder:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  if (firmware_version_valid_) ESP_LOGCONFIG(TAG, "  Firmware version: 0x%02X", firmware_version_);
  else ESP_LOGCONFIG(TAG, "  Firmware version: unavailable");
}

void M5Stack8EncoderResetAllButton::press_action() {
  if (parent_ != nullptr) parent_->reset_all_counters();
}

light::LightTraits M5Stack8EncoderLight::get_traits() {
  light::LightTraits traits;
  traits.set_supported_color_modes({light::ColorMode::RGB});
  return traits;
}
void M5Stack8EncoderLight::write_state(light::LightState *state) {
  float red, green, blue;
  state->current_values_as_rgb(&red, &green, &blue);
  parent_->set_led(index_, uint8_t(red * 255.0f), uint8_t(green * 255.0f), uint8_t(blue * 255.0f));
}

}  // namespace m5stack_8encoder
}  // namespace esphome
