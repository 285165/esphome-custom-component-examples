#include "esphome/core/log.h"
#include "empty_i2c_component.h"

namespace esphome {
namespace empty_i2c_component {

static const char *TAG = "empty_i2c_component.component";
static int i = 9999;

void EmptyI2CComponent::setup() {
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
	ESP_LOGCONFIG(TAG, " component source %s", this->component_source_);
	ESP_LOGCONFIG(TAG, " address x%02x", this->address_);
}


}  // namespace empty_i2c_component
}  // namespace esphome