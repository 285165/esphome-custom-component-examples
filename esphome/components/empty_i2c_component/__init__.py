import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    STATE_CLASS_MEASUREMENT,
    UNIT_METER,
)

DEPENDENCIES = ["i2c"]
CODEOWNERS = ["@285165"]

CONF_MY_SATELLITES  =   'my_satelites'
CONF_MY_ALTITUDE    =   'my_altitude'
CONF_MY_REQUIRED_KEY = 'my_required_key'
CONF_MY_OPTIONAL_KEY = 'my_optional_key'
CONF_I2C_ADDR = 0x01

empty_i2c_component_ns = cg.esphome_ns.namespace('empty_i2c_component')
EmptyI2CComponent = empty_i2c_component_ns.class_('EmptyI2CComponent', cg.Component, i2c.I2CDevice)

MULTI_CONF = True
CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(EmptyI2CComponent),
            cv.Required(CONF_MY_REQUIRED_KEY): cv.string,
            cv.Optional(CONF_MY_OPTIONAL_KEY, default=10): cv.int_,
            cv.Optional(CONF_MY_ALTITUDE): sensor.sensor_schema(
                unit_of_measurement=UNIT_METER,
                accuracy_decimals=1,
            ),
            cv.Optional(CONF_MY_SATELLITES): sensor.sensor_schema(
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(cv.polling_component_schema("30s"))
    .extend(i2c.i2c_device_schema(CONF_I2C_ADDR)),
    cv.only_with_arduino,
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
  
    cg.add(var.set_my_required_key(config[CONF_MY_REQUIRED_KEY]))

    if CONF_MY_OPTIONAL_KEY in config:
        cg.add(var.set_my_optional_key(config[CONF_MY_OPTIONAL_KEY]))

    if CONF_MY_ALTITUDE in config:
        sens = await sensor.new_sensor(config[CONF_MY_ALTITUDE])
        cg.add(var.set_altitude_sensor(sens))

    if CONF_MY_SATELLITES in config:
        sens = await sensor.new_sensor(config[CONF_MY_SATELLITES])
        cg.add(var.set_satellites_sensor(sens))
    
    # https://github.com/lewisxhe/XPowersLib
    cg.add_library("lewisxhe/XPowersLib", "0.2.6")
    cg.add_define("XPOWERS_CHIP_AXP2101")