#include "m5stack_u153.h"
#include "esphome/core/log.h"
namespace esphome {
namespace m5stack_u153 {
static const char *const TAG = "m5stack_u153";

// Kluczowa poprawka: sekwencja write(reg) z STOP, nastepnie osobny read z STOP.
// Odpowiada Wire.write(reg)+endTransmission()+requestFrom() w bibliotece M5Stack.
// ESPHome read_bytes() uzywa repeated-start, ktory zawiesza STM32 U153 -> timeout.
bool M5StackU153::read_regs_(uint8_t reg, uint8_t *data, uint8_t len) {
  // write wskaznika rejestru, stop = true (wymusza warunek STOP)
  if (this->write(&reg, 1, true) != i2c::ERROR_OK)
    return false;
  // osobny odczyt jako nowa transakcja START..STOP
  if (this->read(data, len) != i2c::ERROR_OK)
    return false;
  return true;
}

void M5StackU153::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Stack U153 Unit 8Encoder...");
  uint8_t v = 0;
  fw_ok_ = this->read_regs_(0xF0, &v, 1);
  if (fw_ok_) fw_ = v;
}

void M5StackU153::update() {
  bool ok = true;
  for (uint8_t i = 0; i < 8; i++) {
    if (!encoders_[i]) continue;
    uint8_t d[4];
    uint8_t reg = i * 4;
    if (!this->read_regs_(reg, d, 4)) { ESP_LOGW(TAG, "Failed encoder %u reg 0x%02X", i, reg); ok = false; continue; }
    uint32_t raw = uint32_t(d[0]) | (uint32_t(d[1]) << 8) | (uint32_t(d[2]) << 16) | (uint32_t(d[3]) << 24);
    encoders_[i]->publish_state(float(int32_t(raw)));
  }
  for (uint8_t i = 0; i < 8; i++) {
    if (!buttons_[i]) continue;
    uint8_t v = 0, reg = 0x50 + i;
    if (!this->read_regs_(reg, &v, 1)) { ESP_LOGW(TAG, "Failed button %u reg 0x%02X", i, reg); ok = false; continue; }
    buttons_[i]->publish_state(v == pressed_[i]);
  }
  if (switch_) {
    uint8_t v = 0;
    if (!this->read_regs_(0x60, &v, 1)) { ESP_LOGW(TAG, "Failed switch reg 0x60"); ok = false; }
    else switch_->publish_state(v != 0);
  }
  if (ok) status_clear_warning(); else status_set_warning();
}

bool M5StackU153::set_led(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
  if (i >= 8) return false;
  uint8_t buf[4] = {uint8_t(0x70 + i * 3), r, g, b};
  if (this->write(buf, 4, true) != i2c::ERROR_OK) { ESP_LOGW(TAG, "Failed LED %u", i); return false; }
  return true;
}

void M5StackU153::dump_config() {
  ESP_LOGCONFIG(TAG, "M5Stack U153 Unit 8Encoder:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  if (fw_ok_) ESP_LOGCONFIG(TAG, "  Firmware: 0x%02X", fw_);
  else ESP_LOGCONFIG(TAG, "  Firmware: unavailable");
}

light::LightTraits M5StackU153Light::get_traits() {
  light::LightTraits t;
  t.set_supported_color_modes({light::ColorMode::RGB});
  return t;
}
void M5StackU153Light::write_state(light::LightState *s) {
  float r, g, b;
  s->current_values_as_rgb(&r, &g, &b);
  p_->set_led(i_, uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255));
}
}  // namespace m5stack_u153
}  // namespace esphome
