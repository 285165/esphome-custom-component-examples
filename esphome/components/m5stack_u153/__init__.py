import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, i2c, sensor
from esphome.const import CONF_ID, CONF_NUMBER

CODEOWNERS = []
DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor", "binary_sensor", "light"]
MULTI_CONF = True

CONF_ENCODERS = "encoders"
CONF_BUTTONS = "buttons"
CONF_SWITCH = "switch"
CONF_PRESSED_VALUE = "pressed_value"

m5stack_u153_ns = cg.esphome_ns.namespace("m5stack_u153")
M5StackU153 = m5stack_u153_ns.class_(
    "M5StackU153", cg.PollingComponent, i2c.I2CDevice
)

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
    cv.Optional(CONF_PRESSED_VALUE, default=0): cv.one_of(0, 1, int=True),
})

SWITCH_SCHEMA = binary_sensor.binary_sensor_schema(
    icon="mdi:toggle-switch",
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(M5StackU153),
    cv.Optional(CONF_ENCODERS, default=[]): cv.ensure_list(ENCODER_SCHEMA),
    cv.Optional(CONF_BUTTONS, default=[]): cv.ensure_list(BUTTON_SCHEMA),
    cv.Optional(CONF_SWITCH): SWITCH_SCHEMA,
}).extend(cv.polling_component_schema("200ms")).extend(
    i2c.i2c_device_schema(0x41)
)


def _validate_unique_numbers(config):
    encoder_numbers = [item[CONF_NUMBER] for item in config[CONF_ENCODERS]]
    button_numbers = [item[CONF_NUMBER] for item in config[CONF_BUTTONS]]
    if len(encoder_numbers) != len(set(encoder_numbers)):
        raise cv.Invalid("Each encoder number may be configured only once")
    if len(button_numbers) != len(set(button_numbers)):
        raise cv.Invalid("Each button number may be configured only once")
    return config


CONFIG_SCHEMA = cv.All(CONFIG_SCHEMA, _validate_unique_numbers)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    for item in config[CONF_ENCODERS]:
        entity = await sensor.new_sensor(item)
        cg.add(var.set_encoder_sensor(item[CONF_NUMBER], entity))

    for item in config[CONF_BUTTONS]:
        entity = await binary_sensor.new_binary_sensor(item)
        cg.add(
            var.set_button_sensor(
                item[CONF_NUMBER], entity, item[CONF_PRESSED_VALUE]
            )
        )

    if CONF_SWITCH in config:
        entity = await binary_sensor.new_binary_sensor(config[CONF_SWITCH])
        cg.add(var.set_switch_sensor(entity))
