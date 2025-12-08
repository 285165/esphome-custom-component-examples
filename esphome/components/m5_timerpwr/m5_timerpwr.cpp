#include "m5_timerpwr.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5_timerpwr {

static const char *const TAG = "m5_timerpwr.sensor";

static const uint8_t AXP2101_REGISTER_PMU_STATUS2 = 0x90;
static const uint8_t AXP2101_REGISTER_BATTERY_LEVEL = 0x70;
static const uint8_t TIMERPWR_REGISTER_BATTERY_VOLTAGE = 0x70;
static const uint8_t TIMERPWR_REGISTER_BATTERY_CURRENT = 0x74;
static const uint8_t TIMERPWR_REGISTER_USB_VOLTAGE = 0x60;
static const uint8_t TIMERPWR_REGISTER_USB_CURRENT = 0x64;
static const uint8_t TIMERPWR_REGISTER_GROVE_VOLTAGE = 0x68;
static const uint8_t TIMERPWR_REGISTER_GROVE_CURRENT = 0x6C;

static const uint8_t VERSION = 0xFE;

int32_t convert_array_to_int(uint8_t array[4]) {
  uint8_t bytes[4]{ 0xff, 0xff, 0xff, 0xff };
  int32_t value;
  std::memcpy(&value, array, sizeof(int32_t));
  ESP_LOGD(TAG, "ttt %d", value );
  return value;
}

float M5_timerpwr::get_setup_priority() const { return setup_priority::DATA; }

void M5_timerpwr::update() {
  uint8_t data;

  float battery_level;
  float battery_voltage_f;
  float battery_current_f;
  float usb_voltage_f;
  float usb_current_f;
  float grove_voltage_f;
  float grove_current_f;
  const float V_max = 4.2;
  const float V_min = 3.2;

  uint8_t grove_voltage[4];
  // std::int32_t usb_voltage; 
  if (this->read_register(TIMERPWR_REGISTER_GROVE_VOLTAGE, grove_voltage, 4) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "unable to read GROVE voltage");
      this->mark_failed();
      return;
  } else {
      if (this->grove_voltage_ != nullptr) {
      grove_voltage_f = convert_array_to_int(grove_voltage)/100.0;

      ESP_LOGI(TAG, "GROVE voltage: %.2f", grove_voltage_f );
      this->grove_voltage_->publish_state(grove_voltage_f);
    }
  }
  uint8_t grove_current[4];
  if (this->read_register(TIMERPWR_REGISTER_GROVE_CURRENT, grove_current, 4) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "unable to read GROVE current");
      this->mark_failed();
      return;
  } else {
      if (this->grove_current_ != nullptr) {
      grove_current_f = convert_array_to_int(grove_current)/100.0;
      ESP_LOGI(TAG, "GROVE current: %.2f", grove_current_f );
      this->grove_current_->publish_state(grove_current_f);
    }
  }

  uint8_t usb_voltage[4];
  // std::int32_t usb_voltage; 
  if (this->read_register(TIMERPWR_REGISTER_USB_VOLTAGE, usb_voltage, 4) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "unable to read USB voltage");
      this->mark_failed();
      return;
  } else {
      if (this->usb_voltage_ != nullptr) {
      //usb_voltage_f = (usb_voltage[1]*256+usb_voltage[0])/100.0;
      usb_voltage_f = convert_array_to_int(usb_voltage)/100.0;
      // usb_voltage_f = usb_voltage/100.0;

      ESP_LOGI(TAG, "USB voltage: %.2f", usb_voltage_f );
      // ESP_LOGD(TAG, "USB voltage: %d %d %d %d", usb_voltage[3],usb_voltage[2],usb_voltage[1],usb_voltage[0]);
      this->usb_voltage_->publish_state(usb_voltage_f);
    }
  }
  uint8_t usb_current[4];
  if (this->read_register(TIMERPWR_REGISTER_USB_CURRENT, usb_current, 4) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "unable to read USB current");
      this->mark_failed();
      return;
  } else {
      if (this->usb_current_ != nullptr) {
      //usb_current_f = (65536*usb_current[2]+256*usb_current[1]+usb_current[0])/100.0;
      usb_current_f = convert_array_to_int(usb_current)/100.0;
      // usb_current_f = (int16_t*)usb_current/100.0;
      ESP_LOGI(TAG, "USB current: %.2f", usb_current_f );
      ESP_LOGD(TAG, "USB current: %d %d %d %d", usb_current[3],usb_current[2],usb_current[1],usb_current[0]);
      this->usb_current_->publish_state(usb_current_f);
    }
  }

  uint8_t battery_voltage[4];
  if (this->read_register(TIMERPWR_REGISTER_BATTERY_VOLTAGE, battery_voltage, 4) != i2c::NO_ERROR) {
    ESP_LOGE(TAG, "Unable to read from device");
    return;
  } else {
      if (this->battery_voltage_ != nullptr) {
      // battery_voltage_f = (65536*battery_voltage[2]+256*battery_voltage[1]+battery_voltage[0])/100.0;
      battery_voltage_f = convert_array_to_int(battery_voltage)/100.0;
      ESP_LOGI(TAG, "Battery voltage read: %.2f", battery_voltage_f );
      ESP_LOGD(TAG, "Battery voltage: %d %d %d %d", battery_voltage[3],battery_voltage[2],battery_voltage[1],battery_voltage[0]);
      this->battery_voltage_->publish_state(battery_voltage_f);  
    }
  }

  uint8_t battery_current[4];
  if (this->read_register(TIMERPWR_REGISTER_BATTERY_CURRENT, battery_current, 4) != i2c::NO_ERROR) {
    ESP_LOGE(TAG, "Unable to read from device");
    return;
  } else {
      if (this->battery_current_ != nullptr) {
      battery_current_f = (65536*battery_current[2]+256*battery_current[1]+battery_current[0])/100.0;
      // battery_current_f = (int16_t*)battery_current/100.0;
      battery_current_f = convert_array_to_int(battery_current)/100.0;
      //battery_current_f = value/100.0;

      ESP_LOGI(TAG, "Battery current read: %.2f",battery_current_f );
      ESP_LOGD(TAG, "Battery current: %d %d %d %d", battery_current[3],battery_current[2],battery_current[1],battery_current[0]);
      this->battery_current_->publish_state(battery_current_f);  
    }
  }

 if (this->read_register(AXP2101_REGISTER_BATTERY_LEVEL, &data, 1) != i2c::NO_ERROR) {
    ESP_LOGE(TAG, "Unable to read from device");
    return;
  }
  if (this->battery_level_ != nullptr)
    battery_level = ((battery_voltage_f - V_min) / (V_max - V_min)) * 100;
    ESP_LOGI(TAG, "Battery level: %.2f", battery_level );
    this->battery_level_->publish_state(battery_level);
}

void M5_timerpwr::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5_timerpwr...");
  if (this->read_register(VERSION, &this->version_, 1) != i2c::ERROR_OK) {
    this->mark_failed();
    return;
  }

  ESP_LOGCONFIG(TAG, "Firmware version %d", this->version_);
  // uint8_t data[8] = {1,1,1,1,1,1,1,1};
  // this->write_register(RESET_BASE, data, 8);
}

void M5_timerpwr::loop() {
  uint8_t data;
  if (this->charging_ != nullptr) {
    if (this->read_register(AXP2101_REGISTER_PMU_STATUS2, &data, 1) != i2c::NO_ERROR)
      return;
    this->charging_->publish_state((data & 0x01) == 0x01);
  }
}

void M5_timerpwr::dump_config() {
  ESP_LOGCONFIG(TAG, "M5_timerpwr:");
  if (this->version_ > 0)
    ESP_LOGCONFIG(TAG, "Firmware version %d", this->version_);
  else
    ESP_LOGCONFIG(TAG, "setup failed");
    
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Connection with M5_timerpwr failed!");
  }
  // LOG_UPDATE_INTERVAL(this);
  // LOG_SENSOR(" ", "Battery Level", this->battery_level_);
  // LOG_SENSOR(" ", "Battery Voltage", this->battery_voltage_);
  // LOG_SENSOR(" ", "Battery Current", this->battery_current_);
  // LOG_SENSOR(" ", "USB Voltage", this->usb_voltage_);
  // LOG_SENSOR(" ", "USB Current", this->usb_current_);
}

}  // namespace m5_timerpwr
}  // namespace esphome
