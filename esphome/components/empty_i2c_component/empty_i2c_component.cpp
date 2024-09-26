#include "esphome/core/log.h"
#include "empty_i2c_component.h"
#include <XPowersLib.h>
#include <Wire.h>
#include <Arduino.h>
#include <experimental/random>

#ifndef CONFIG_PMU_SDA
#define CONFIG_PMU_SDA 21
#endif

#ifndef CONFIG_PMU_SCL
#define CONFIG_PMU_SCL 22
#endif

#define XPOWERS_CHIP_AXP2101

namespace esphome {
namespace empty_i2c_component {

static const char *TAG = "empty_i2c_component.component";
static int i = std::numeric_limits<int>::max()-1;
//static XPowersPMU power;

EmptyI2CComponent::EmptyI2CComponent() {}

// void EmptyI2CComponent::setup() {
// 	const uint8_t i2c_sda = CONFIG_PMU_SDA;
// 	const uint8_t i2c_scl = CONFIG_PMU_SCL;
	
// 	this->set_component_source("esphome.components.EmptyI2CComponent");
// /*	
// 	bool result = power.begin(Wire, AXP2101_SLAVE_ADDRESS, i2c_sda, i2c_scl);
//     if (result == false) {
//         ESP_LOGD("power is not online...");
//     }
// */	
// 	ESP_LOGD(TAG, "Setting up EmptyI2CComponent...");
// 	ESP_LOGD(TAG, "	address x%02x", this->address_);
// 	ESP_LOGD(TAG, "	my_required_key: %s", this->my_required_key_);
// 	ESP_LOGD(TAG, "	my_optional_key: %d", this->my_optional_key_);
// }

void EmptyI2CComponent::update() {
  ESP_LOGI(TAG, " component state: x%08x",this->component_state_);
  if (this->my_altitude_sensor_ != nullptr)
    this->my_altitude_sensor_->publish_state(this->altitude_);

  if (this->satelites_sensor_ != nullptr)
    this->satelites_sensor_->publish_state(this->satellites_);
}

void EmptyI2CComponent::loop() {
	while (this->available() && !this->has_time_) {
		if (true /*tiny_gps_.altitude.isUpdated()*/) {
			this->altitude_ = static_cast <float> (1000*std::experimental::randint(0, 100)) / static_cast <float> (100); //tiny_gps_.altitude.meters();
			ESP_LOGD(TAG, "Altitude:");
			ESP_LOGD(TAG, "  %f m", this->altitude_);
      	}
		if (true /*tiny_gps_.satellites.isUpdated()*/) {
			this->satellites_ = std::experimental::randint(1, 12);
			ESP_LOGD(TAG, "Satellites:");
			ESP_LOGD(TAG, "  %d", this->satellites_);
		}
	}
  }
	// if (i >= this->my_optional_key_) {
	// 	i = 0;
	// 	ESP_LOGD(TAG, "EmptyI2CComponent::loop");
	// } else {
	// 	if ( i%1000 == 0) {
	// 	  status_set_warning("testowe ostrzeżenie");
	//       ESP_LOGI(TAG, " component state: x%08x",this->component_state_);
	// 	}
	// 	if ( i%1000 == 501) {
	// 	  status_clear_warning();
	//   	  ESP_LOGI(TAG, " component state: x%08x",this->component_state_);
	// 	}
	// 	i++;
	// }


// void EmptyI2CComponent::dump_config(){
//     ESP_LOGCONFIG(TAG, "Empty I2C component");
// 	ESP_LOGCONFIG(TAG, " component source: %s", this->component_source_);
// 	ESP_LOGCONFIG(TAG, " component state: x%08x",this->component_state_);
// 	ESP_LOGCONFIG(TAG, " address: x%02x", this->address_);
// 	ESP_LOGCONFIG(TAG, " my_required_key: %s", this->my_required_key_.c_str());
// 	ESP_LOGCONFIG(TAG, " my_optional_key: %d", this->my_optional_key_);
// }


}  // namespace empty_i2c_component
}  // namespace esphom{