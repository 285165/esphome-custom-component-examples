#include "esphome/core/log.h"
#include "empty_i2c_component.h"

namespace esphome {
namespace empty_i2c_component {

static const char *TAG = "empty_i2c_component.component";
static int i = 0;

void EmptyI2CComponent::setup() {

}

void EmptyI2CComponent::loop() {
	if (++i > 1000) {
		i = 0;
		ESP_LOGD(TAG, "EmptyI2CComponent::loop");
	}
}

void EmptyI2CComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "Empty I2C component");
	ESP_LOGCONFIG(TAG, "	CONF_I2C_ADDR: %02x",0xff);
}


}  // namespace empty_i2c_component
}  // namespace esphome