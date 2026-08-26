import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, i2c, sensor
from esphome.const import CONF_ID, CONF_NUMBER

CODEOWNERS = []
DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor", "binary_sensor", "light"]
MULTI_CONF = True

CONF_CHANNELS = "channels"
CONF_BIT_DEPTH = "bit_depth"
CONF_SW = "sw"
CONF_CHANGE_I2C_ADDRESS_TO = "change_i2c_address_to"

m5stack_8angle_ns = cg.esphome_ns.namespace("m5stack_8angle")
M5Stack8Angle = m5stack_8angle_ns.class_(
    "M5Stack8Angle", cg.PollingComponent, i2c.I2CDevice
)

BIT_DEPTHS = {"8bit": 8, "12bit": 12}

CHANNEL_SCHEMA = sensor.sensor_schema(
    accuracy_decimals=0,
    icon="mdi:knob",
).extend({
    cv.Required(CONF_NUMBER): cv.int_range(min=0, max=7),
    cv.Optional(CONF_BIT_DEPTH, default="12bit"): cv.enum(BIT_DEPTHS, lower=True),
})

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(M5Stack8Angle),
    cv.Optional(CONF_CHANGE_I2C_ADDRESS_TO): cv.i2c_address,
    cv.Optional(CONF_CHANNELS, default=[]): cv.ensure_list(CHANNEL_SCHEMA),
    cv.Optional(CONF_SW): binary_sensor.binary_sensor_schema(icon="mdi:toggle-switch"),
}).extend(cv.polling_component_schema("100ms")).extend(
    i2c.i2c_device_schema(0x43)
)


def _validate(config):
    numbers = [item[CONF_NUMBER] for item in config[CONF_CHANNELS]]
    if len(numbers) != len(set(numbers)):
        raise cv.Invalid("Each channel number may be configured only once")
    if CONF_CHANGE_I2C_ADDRESS_TO in config:
        if config[CONF_CHANGE_I2C_ADDRESS_TO] == config.get("address", 0x43):
            raise cv.Invalid("change_i2c_address_to must differ from address")
    return config

CONFIG_SCHEMA = cv.All(CONFIG_SCHEMA, _validate)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_CHANGE_I2C_ADDRESS_TO in config:
        cg.add(var.set_change_i2c_address_to(config[CONF_CHANGE_I2C_ADDRESS_TO]))

    for item in config[CONF_CHANNELS]:
        entity = await sensor.new_sensor(item)
        cg.add(var.set_channel_sensor(item[CONF_NUMBER], entity, item[CONF_BIT_DEPTH]))

    if CONF_SW in config:
        entity = await binary_sensor.new_binary_sensor(config[CONF_SW])
        cg.add(var.set_sw_sensor(entity))
