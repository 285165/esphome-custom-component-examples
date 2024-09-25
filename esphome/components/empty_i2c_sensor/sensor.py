import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
#from ..bmp280_base import to_code_base, CONFIG_SCHEMA_BASE
from esphome.const import CONF_ID #, ICON_EMPTY, UNIT_EMPTY

#AUTO_LOAD = ["bmp280_base"]
CODEOWNERS = ["@285165"]
DEPENDENCIES = ['i2c']

CONF_I2C_ADDR = 0x01

empty_i2c_sensor_ns = cg.esphome_ns.namespace('empty_i2c_sensor')
EmptyI2CSensor = empty_i2c_sensor_ns.class_('EmptyI2CSensor', cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(EmptyI2CSensor),
#    cv.Optional(CONF_BUS_VOLTAGE): sensor.sensor_schema(UNIT_VOLT, ICON_FLASH, 2),
#    cv.Optional(CONF_SHUNT_VOLTAGE): sensor.sensor_schema(UNIT_VOLT, ICON_FLASH, 2),
#    cv.Optional(CONF_CURRENT): sensor.sensor_schema(UNIT_AMPERE, ICON_FLASH, 3),
#    cv.Optional(CONF_POWER): sensor.sensor_schema(UNIT_WATT, ICON_FLASH, 2),
#    cv.Optional(CONF_SHUNT_RESISTANCE, default=0.1): cv.All(cv.resistance, cv.Range(min=0.0)),
#    cv.Optional(CONF_MAX_CURRENT, default=3.2): cv.All(cv.current, cv.Range(min=0.0)),
}).extend(cv.polling_component_schema('60s')).extend(i2c.i2c_device_schema(CONF_I2C_ADDR))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)
    yield i2c.register_i2c_device(var, config)