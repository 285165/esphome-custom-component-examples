#include "esphome/core/log.h"
#include "empty_i2c_component.h"

namespace esphome {
namespace empty_i2c_component {

static const char *TAG = "empty_i2c_component.component";
static int i = 9999;

void EmptyI2CComponent::setup() {
	ESP_LOGCONFIG(TAG, "Setting up EmptyI2CComponent...");
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
	ESP_LOGCONFIG(TAG, "	CONF_I2C_ADDR: %02x",0xff);
}


}  // namespace empty_i2c_component
}  // namespace esphome