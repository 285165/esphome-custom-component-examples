#include "m5stack_8encoder.h"

#include "esphome/core/log.h"

namespace esphome {
namespace m5stack_8encoder {

static const char *const TAG = "m5stack_8encoder";

void M5Stack8Encoder::set_encoder_sensor(uint8_t index, sensor::Sensor *entity) {
  if (index < this->encoder_sensors_.size())
    this->encoder_sensors_[index] = entity;
}

void M5Stack8Encoder::set_button_sensor(
    uint8_t index, binary_sensor::BinarySensor *entity, uint8_t pressed_value) {
  if (index < this->button_sensors_.size()) {
    this->button_sensors_[index] = entity;
    this->button_pressed_values_[index] = pressed_value;
  }
}

bool M5Stack8Encoder::read_regs_(uint8_t reg, uint8_t *data, uint8_t len) {
  if (this->write(&reg, 1) != i2c::ERROR_OK)
    return false;
  return this->read(data, len) == i2c::ERROR_OK;
}

bool M5Stack8Encoder::write_int32_(uint8_t reg, int32_t value) {
  const uint32_t raw = static_cast<uint32_t>(value);
  const uint8_t data[5] = {
      reg,
      static_cast<uint8_t>(raw),
      static_cast<uint8_t>(raw >> 8),
      static_cast<uint8_t>(raw >> 16),
      static_cast<uint8_t>(raw >> 24),
  };
  return this->write(data, sizeof(data)) == i2c::ERROR_OK;
}

bool M5Stack8Encoder::configure_increments_() {
  // One signed 32-bit little-endian increment per channel in 0x20..0x3F.
  for (uint8_t channel = 0; channel < 8; channel++) {
    if (!this->write_int32_(REG_INCREMENT_BASE + channel * 4,
                            this->increment_)) {
      ESP_LOGE(TAG, "Failed to set increment for CH%u", channel + 1);
      return false;
    }
  }
  return true;
}

bool M5Stack8Encoder::reset_all_counters() {
  if (!this->present_ || this->is_failed())
    return false;

  // Reset registers 0x40..0x4F in one I2C write transaction.
  // Byte 0 is the starting register, followed by 16 zero bytes.
  uint8_t data[17] = {REG_RESET_BASE};
  if (this->write(data, sizeof(data)) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Failed to reset all counters");
    return false;
  }

  for (auto *entity : this->encoder_sensors_) {
    if (entity != nullptr)
      entity->publish_state(0.0f);
  }
  ESP_LOGI(TAG, "All CH1..CH8 counters reset");
  return true;
}

bool M5Stack8Encoder::change_device_address_() {
  const uint8_t data[2] = {REG_I2C_ADDRESS, this->new_i2c_address_};
  if (this->write(data, sizeof(data)) != i2c::ERROR_OK)
    return false;

  ESP_LOGW(TAG, "I2C address changed from 0x%02X to 0x%02X",
           this->address_, this->new_i2c_address_);
  this->set_i2c_address(this->new_i2c_address_);
  return true;
}

void M5Stack8Encoder::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Stack Unit 8Encoder...");

  uint8_t reg = REG_COUNTER_BASE;
  if (this->write(&reg, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Unit 8Encoder not found at address 0x%02X", this->address_);
    this->mark_failed();
    return;
  }
  this->present_ = true;

  uint8_t version = 0;
  this->firmware_version_valid_ =
      this->read_regs_(REG_FIRMWARE_VERSION, &version, 1);
  if (this->firmware_version_valid_)
    this->firmware_version_ = version;

  if (!this->configure_increments_()) {
    this->mark_failed();
    return;
  }

  if (this->change_i2c_address_ && !this->change_device_address_()) {
    ESP_LOGE(TAG, "Failed to change I2C address");
    this->mark_failed();
  }
}

void M5Stack8Encoder::update() {
  if (!this->present_ || this->is_failed())
    return;

  bool all_ok = true;

  for (uint8_t channel = 0; channel < 8; channel++) {
    if (this->encoder_sensors_[channel] == nullptr)
      continue;

    uint8_t data[4];
    const uint8_t reg = REG_COUNTER_BASE + channel * 4;
    if (!this->read_regs_(reg, data, sizeof(data))) {
      ESP_LOGW(TAG, "Failed to read CH%u counter", channel + 1);
      all_ok = false;
      continue;
    }

    const uint32_t raw = static_cast<uint32_t>(data[0]) |
                         (static_cast<uint32_t>(data[1]) << 8) |
                         (static_cast<uint32_t>(data[2]) << 16) |
                         (static_cast<uint32_t>(data[3]) << 24);
    this->encoder_sensors_[channel]->publish_state(
        static_cast<float>(static_cast<int32_t>(raw)));
  }

  for (uint8_t channel = 0; channel < 8; channel++) {
    if (this->button_sensors_[channel] == nullptr)
      continue;

    uint8_t value = 0;
    if (!this->read_regs_(REG_BUTTON_BASE + channel, &value, 1)) {
      ESP_LOGW(TAG, "Failed to read CH%u button", channel + 1);
      all_ok = false;
      continue;
    }
    this->button_sensors_[channel]->publish_state(
        value == this->button_pressed_values_[channel]);
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

bool M5Stack8Encoder::set_led(uint8_t index, uint8_t red, uint8_t green,
                              uint8_t blue) {
  if (!this->present_ || this->is_failed() || index >= 9)
    return false;

  const uint8_t data[4] = {
      static_cast<uint8_t>(REG_RGB_BASE + index * 3), red, green, blue};
  return this->write(data, sizeof(data)) == i2c::ERROR_OK;
}

void M5Stack8Encoder::dump_config() {
  ESP_LOGCONFIG(TAG, "M5Stack Unit 8Encoder:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  ESP_LOGCONFIG(TAG, "  Increment CH1..CH8: %" PRId32, this->increment_);
  if (this->firmware_version_valid_)
    ESP_LOGCONFIG(TAG, "  Firmware version: 0x%02X", this->firmware_version_);
  else
    ESP_LOGCONFIG(TAG, "  Firmware version: unavailable");
}

void M5Stack8EncoderResetAllButton::press_action() {
  if (this->parent_ != nullptr)
    this->parent_->reset_all_counters();
}

light::LightTraits M5Stack8EncoderLight::get_traits() {
  light::LightTraits traits;
  traits.set_supported_color_modes({light::ColorMode::RGB});
  return traits;
}

void M5Stack8EncoderLight::write_state(light::LightState *state) {
  float red;
  float green;
  float blue;
  state->current_values_as_rgb(&red, &green, &blue);
  this->parent_->set_led(
      this->index_, static_cast<uint8_t>(red * 255.0f),
      static_cast<uint8_t>(green * 255.0f),
      static_cast<uint8_t>(blue * 255.0f));
}

}  // namespace m5stack_8encoder
}  // namespace esphome
