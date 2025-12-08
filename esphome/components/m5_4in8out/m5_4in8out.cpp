#include "m5_4in8out.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5_4in8out {

static const char *const TAG = "M5Stack4In8Out";

void M5Stack4In8OutComponent::setup() {
  if (!this->read_gpio_()) {
    ESP_LOGE(TAG, "M5Stack 4in8out not available under 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  this->write_gpio_();
  this->read_gpio_();
}
void M5Stack4In8OutComponent::loop() {
  // Invalidate the cache at the start of each loop
  this->reset_pin_cache_();
}
void M5Stack4In8OutComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "M5Stack4In8Out:");
  LOG_I2C_DEVICE(this)
  if (this->is_failed()) {
    ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL);
  }
}
bool M5Stack4In8OutComponent::digital_read_hw(uint8_t pin) {
  // Read all pins from hardware into input_mask_
  return this->read_gpio_();  // Return true if I2C read succeeded, false on error
}

bool M5Stack4In8OutComponent::digital_read_cache(uint8_t pin) { return this->input_mask_ & (1 << pin); }

void M5Stack4In8OutComponent::digital_write_hw(uint8_t pin, bool value) {
  if (value) {
    this->output_mask_ |= (1 << pin);
  } else {
    this->output_mask_ &= ~(1 << pin);
  }
  this->write_gpio_();
}
void M5Stack4In8OutComponent::pin_mode(uint8_t pin, gpio::Flags flags) {
  if (flags == gpio::FLAG_INPUT) {
    // Clear mode mask bit
    this->mode_mask_ &= ~(1 << pin);
    // Write GPIO to enable input mode
    this->write_gpio_();
  } else if (flags == gpio::FLAG_OUTPUT) {
    // Set mode mask bit
    this->mode_mask_ |= 1 << pin;
  }
}
bool M5Stack4In8OutComponent::read_gpio_() {
  if (this->is_failed())
    return false;
  bool success;
  uint8_t data[2];

    success = this->read_bytes_raw(data, 2);
    this->input_mask_ = (uint16_t(data[1]) << 8) | (uint16_t(data[0]) << 0);


  if (!success) {
    this->status_set_warning();
    return false;
  }
  this->status_clear_warning();
  return true;
}
bool M5Stack4In8OutComponent::write_gpio_() {
  if (this->is_failed())
    return false;

  uint16_t value = 0;
  // Pins in OUTPUT mode and where pin is HIGH.
  value |= this->mode_mask_ & this->output_mask_;
  // Pins in INPUT mode must also be set here
  value |= ~this->mode_mask_;

  uint8_t data[2];
  data[0] = value;
  data[1] = value >> 8;
  if (this->write(data, 2) != i2c::ERROR_OK) {
    this->status_set_warning();
    return false;
  }

  this->status_clear_warning();
  return true;
}
float M5Stack4In8OutComponent::get_setup_priority() const { return setup_priority::IO; }

// Run our loop() method early to invalidate cache before any other components access the pins
float M5Stack4In8OutComponent::get_loop_priority() const { return 9.0f; }  // Just after WIFI

void M5Stack4In8OutGPIOPin::setup() { pin_mode(flags_); }
void M5Stack4In8OutGPIOPin::pin_mode(gpio::Flags flags) { this->parent_->pin_mode(this->pin_, flags); }
bool M5Stack4In8OutGPIOPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }
void M5Stack4In8OutGPIOPin::digital_write(bool value) { this->parent_->digital_write(this->pin_, value != this->inverted_); }
std::string M5Stack4In8OutGPIOPin::dump_summary() const {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%u via M5Stack 4in8out", pin_);
  return buffer;
}

}  // namespace m5_4in8out
}  // namespace esphome