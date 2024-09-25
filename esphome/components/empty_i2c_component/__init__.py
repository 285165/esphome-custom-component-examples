import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID

DEPENDENCIES = ['i2c']

CONF_MY_REQUIRED_KEY = 'my_required_key'
CONF_MY_OPTIONAL_KEY = 'my_optional_key'
CONF_I2C_ADDR = 0x01

empty_i2c_component_ns = cg.esphome_ns.namespace('empty_i2c_component')
EmptyI2CComponent = empty_i2c_component_ns.class_('EmptyI2CComponent', cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(EmptyI2CComponent),
    cv.Required(CONF_MY_REQUIRED_KEY): cv.string,
    cv.Optional(CONF_MY_OPTIONAL_KEY, default=10): cv.int_,
}).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(CONF_I2C_ADDR))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)
    
    cg.add(var.set_my_required_key(config[CONF_MY_REQUIRED_KEY]))
    cg.add(var.set_my_required_key(config[CONF_MY_OPTIONAL_KEY]))
    
    # https://github.com/lewisxhe/XPowersLib
    cg.add_library("lewisxhe/XPowersLib", "0.2.6")