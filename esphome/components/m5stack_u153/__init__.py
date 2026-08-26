import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor, binary_sensor
from esphome.const import CONF_ID, CONF_NUMBER

CODEOWNERS = []
DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor", "binary_sensor"]
MULTI_CONF = True

CONF_ENCODERS = "encoders"
CONF_BUTTONS = "buttons"
CONF_TOGGLE = "toggle"
CONF_INVERTED = "inverted"

m5stack_u153_ns = cg.esphome_ns.namespace("m5stack_u153")
M5StackU153 = m5stack_u153_ns.class_("M5StackU153", cg.PollingComponent, i2c.I2CDevice)

ENCODER_SCHEMA = sensor.sensor_schema(
    accuracy_decimals=0,
    icon="mdi:knob",
).extend({
    cv.Required(CONF_NUMBER): cv.int_range(min=0, max=7),
})

BUTTON_SCHEMA = binary_sensor.binary_sensor_schema(
    icon="mdi:gesture-tap-button",
).extend({
    cv.Required(CONF_NUMBER): cv.int_range(min=0, max=7),
    cv.Optional(CONF_INVERTED, default=True): cv.boolean,
})

TOGGLE_SCHEMA = binary_sensor.binary_sensor_schema(
    icon="mdi:toggle-switch",
).extend({
    cv.Optional(CONF_INVERTED, default=False): cv.boolean,
})

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(M5StackU153),
    cv.Optional(CONF_ENCODERS, default=[]): cv.ensure_list(ENCODER_SCHEMA),
    cv.Optional(CONF_BUTTONS, default=[]): cv.ensure_list(BUTTON_SCHEMA),
    cv.Optional(CONF_TOGGLE): TOGGLE_SCHEMA,
}).extend(cv.polling_component_schema("50ms")).extend(i2c.i2c_device_schema(0x41))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    for item in config[CONF_ENCODERS]:
        sens = await sensor.new_sensor(item)
        cg.add(var.set_encoder_sensor(item[CONF_NUMBER], sens))

    for item in config[CONF_BUTTONS]:
        btn = await binary_sensor.new_binary_sensor(item)
        cg.add(var.set_button_sensor(item[CONF_NUMBER], btn, item[CONF_INVERTED]))

    if CONF_TOGGLE in config:
        item = config[CONF_TOGGLE]
        toggle = await binary_sensor.new_binary_sensor(item)
        cg.add(var.set_toggle_sensor(toggle, item[CONF_INVERTED]))
