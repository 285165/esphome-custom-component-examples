#pragma once
#include <array>
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
namespace esphome { namespace m5stack_u153 {
class M5StackU153: public PollingComponent, public i2c::I2CDevice {
 public:
 void setup() override; void update() override; void dump_config() override;
 float get_setup_priority() const override { return setup_priority::DATA; }
 void set_encoder_sensor(uint8_t i,sensor::Sensor *s){if(i<8) encoders_[i]=s;}
 void set_button_sensor(uint8_t i,binary_sensor::BinarySensor *s,uint8_t p){if(i<8){buttons_[i]=s;pressed_[i]=p;}}
 void set_switch_sensor(binary_sensor::BinarySensor *s){switch_=s;}
 bool set_led(uint8_t i,uint8_t r,uint8_t g,uint8_t b);
 protected:
 std::array<sensor::Sensor*,8> encoders_{}; std::array<binary_sensor::BinarySensor*,8> buttons_{}; std::array<uint8_t,8> pressed_{}; binary_sensor::BinarySensor *switch_{nullptr}; uint8_t fw_{0}; bool fw_ok_{false};
};
class M5StackU153Light: public light::LightOutput { public: M5StackU153Light(M5StackU153*p,uint8_t i):p_(p),i_(i){} light::LightTraits get_traits() override; void write_state(light::LightState*s) override; protected:M5StackU153*p_;uint8_t i_;};
}}
