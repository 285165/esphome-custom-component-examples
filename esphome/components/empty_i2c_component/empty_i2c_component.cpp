#include "esphome/core/log.h"
#include "empty_i2c_component.h"
#include <XPowersLib.h>

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
static int i = 9999;

void EmptyI2CComponent::setup() {
	const uint8_t i2c_sda = CONFIG_PMU_SDA;
	const uint8_t i2c_scl = CONFIG_PMU_SCL;
	XPowersPMU power;
	
	bool result = power.begin(Wire, AXP2101_SLAVE_ADDRESS, i2c_sda, i2c_scl);
    if (result == false) {
        ESP_LOGD("power is not online...");
    }
	
	ESP_LOGD(TAG, "Setting up EmptyI2CComponent...");
	ESP_LOGD(TAG, "	address x%02x", this->address_);
}

void EmptyI2CComponent::loop() {
	if (i >= 10000) {
		i = 0;
		ESP_LOGD(TAG, "EmptyI2CComponent::loop");
	} else {
		i++;
	}
}

void EmptyI2CComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "Empty I2C component");
	ESP_LOGCONFIG(TAG, " component source: %s", this->component_source_);
	ESP_LOGCONFIG(TAG, " address: x%02x", this->address_);
}


}  // namespace empty_i2c_component
}  // namespace esphome