import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, button, i2c, sensor
from esphome.const import CONF_ID, CONF_NUMBER

CODEOWNERS = []
DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor", "binary_sensor", "button", "light"]
MULTI_CONF = True

CONF_ENCODERS = "encoders"
CONF_INCREMENTS = "increments"
CONF_BUTTONS = "buttons"
CONF_SW = "sw"
CONF_RESET_ALL = "reset_all"
CONF_PRESSED_VALUE = "pressed_value"
CONF_CHANGE_I2C_ADDRESS_TO = "change_i2c_address_to"

m5stack_8encoder_ns = cg.esphome_ns.namespace("m5stack_8encoder")
M5Stack8Encoder = m5stack_8encoder_ns.class_(
    "M5Stack8Encoder", cg.PollingComponent, i2c.I2CDevice
)
M5Stack8EncoderResetAllButton = m5stack_8encoder_ns.class_(
    "M5Stack8EncoderResetAllButton", button.Button
)

CHANNEL_SENSOR_SCHEMA = sensor.sensor_schema(
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

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(M5Stack8Encoder),
    cv.Optional(CONF_CHANGE_I2C_ADDRESS_TO): cv.i2c_address,
    cv.Optional(CONF_ENCODERS, default=[]): cv.ensure_list(CHANNEL_SENSOR_SCHEMA),
    cv.Optional(CONF_INCREMENTS, default=[]): cv.ensure_list(CHANNEL_SENSOR_SCHEMA),
    cv.Optional(CONF_BUTTONS, default=[]): cv.ensure_list(BUTTON_SCHEMA),
    cv.Optional(CONF_SW): binary_sensor.binary_sensor_schema(icon="mdi:toggle-switch"),
    cv.Optional(CONF_RESET_ALL): button.button_schema(
        M5Stack8EncoderResetAllButton, icon="mdi:counter"
    ),
}).extend(cv.polling_component_schema("200ms")).extend(
    i2c.i2c_device_schema(0x41)
)


def _validate(config):
    for key in (CONF_ENCODERS, CONF_INCREMENTS, CONF_BUTTONS):
        numbers = [item[CONF_NUMBER] for item in config[key]]
        if len(numbers) != len(set(numbers)):
            raise cv.Invalid(f"Each number in {key} may be configured only once")
    if CONF_CHANGE_I2C_ADDRESS_TO in config:
        if config[CONF_CHANGE_I2C_ADDRESS_TO] == config.get("address", 0x41):
            raise cv.Invalid("change_i2c_address_to must differ from address")
    return config

CONFIG_SCHEMA = cv.All(CONFIG_SCHEMA, _validate)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_CHANGE_I2C_ADDRESS_TO in config:
        cg.add(var.set_change_i2c_address_to(config[CONF_CHANGE_I2C_ADDRESS_TO]))

    for item in config[CONF_ENCODERS]:
        entity = await sensor.new_sensor(item)
        cg.add(var.set_encoder_sensor(item[CONF_NUMBER], entity))

    for item in config[CONF_INCREMENTS]:
        entity = await sensor.new_sensor(item)
        cg.add(var.set_increment_sensor(item[CONF_NUMBER], entity))

    for item in config[CONF_BUTTONS]:
        entity = await binary_sensor.new_binary_sensor(item)
        cg.add(var.set_button_sensor(
            item[CONF_NUMBER], entity, item[CONF_PRESSED_VALUE]
        ))

    if CONF_SW in config:
        entity = await binary_sensor.new_binary_sensor(config[CONF_SW])
        cg.add(var.set_sw_sensor(entity))

    if CONF_RESET_ALL in config:
        entity = await button.new_button(config[CONF_RESET_ALL])
        cg.add(entity.set_parent(var))
