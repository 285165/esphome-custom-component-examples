#include "m5stack_u153.h"

#include "esphome/core/log.h"

namespace esphome {
namespace m5stack_u153 {

static const char *const TAG = "m5stack_u153";

bool M5StackU153::read_regs_(uint8_t reg, uint8_t *data, uint8_t len) {
  // I2CDevice::write() finishes with STOP. The following read() starts a new
  // transaction. This matches the transaction sequence accepted by U153.
  if (this->write(&reg, 1) != i2c::ERROR_OK)
    return false;
  return this->read(data, len) == i2c::ERROR_OK;
}

void M5StackU153::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Stack U153 Unit 8Encoder...");

  uint8_t reg = REG_COUNTER_BASE;
  if (this->write(&reg, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG,
             "U153 not found on I2C bus at address 0x%02X; component disabled",
             this->address_);
    this->mark_failed();
    return;
  }
  this->present_ = true;

  uint8_t version = 0;
  this->firmware_version_valid_ =
      this->read_regs_(REG_FIRMWARE_VERSION, &version, 1);
  if (this->firmware_version_valid_)
    this->firmware_version_ = version;
}

void M5StackU153::update() {
  if (!this->present_ || this->is_failed())
    return;

  bool all_ok = true;

  for (uint8_t index = 0; index < 8; index++) {
    if (this->encoder_sensors_[index] == nullptr)
      continue;

    uint8_t data[4];
    const uint8_t reg = REG_COUNTER_BASE + index * 4;
    if (!this->read_regs_(reg, data, sizeof(data))) {
      ESP_LOGW(TAG, "Failed to read encoder %u at register 0x%02X", index, reg);
      all_ok = false;
      continue;
    }

    const uint32_t raw = static_cast<uint32_t>(data[0]) |
                         (static_cast<uint32_t>(data[1]) << 8) |
                         (static_cast<uint32_t>(data[2]) << 16) |
                         (static_cast<uint32_t>(data[3]) << 24);
    this->encoder_sensors_[index]->publish_state(
        static_cast<float>(static_cast<int32_t>(raw)));
  }

  for (uint8_t index = 0; index < 8; index++) {
    if (this->button_sensors_[index] == nullptr)
      continue;

    uint8_t value = 0;
    const uint8_t reg = REG_BUTTON_BASE + index;
    if (!this->read_regs_(reg, &value, 1)) {
      ESP_LOGW(TAG, "Failed to read button %u at register 0x%02X", index, reg);
      all_ok = false;
      continue;
    }

    this->button_sensors_[index]->publish_state(
        value == this->button_pressed_values_[index]);
  }

  if (this->switch_sensor_ != nullptr) {
    uint8_t value = 0;
    if (!this->read_regs_(REG_SWITCH, &value, 1)) {
      ESP_LOGW(TAG, "Failed to read toggle switch at register 0x%02X",
               REG_SWITCH);
      all_ok = false;
    } else {
      this->switch_sensor_->publish_state(value != 0);
    }
  }

  if (all_ok)
    this->status_clear_warning();
  else
    this->status_set_warning();
}

bool M5StackU153::set_led(uint8_t index,
                          uint8_t red,
                          uint8_t green,
                          uint8_t blue) {
  if (!this->present_ || this->is_failed() || index >= 8)
    return false;

  const uint8_t data[4] = {
      static_cast<uint8_t>(REG_RGB_BASE + index * 3), red, green, blue};
  if (this->write(data, sizeof(data)) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to write LED %u", index);
    return false;
  }
  return true;
}

void M5StackU153::dump_config() {
  ESP_LOGCONFIG(TAG, "M5Stack U153 Unit 8Encoder:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);

  if (this->is_failed()) {
    ESP_LOGE(TAG, "  Communication failed; component disabled");
    return;
  }

  if (this->firmware_version_valid_)
    ESP_LOGCONFIG(TAG, "  Firmware: 0x%02X", this->firmware_version_);
  else
    ESP_LOGCONFIG(TAG, "  Firmware: unavailable");
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
