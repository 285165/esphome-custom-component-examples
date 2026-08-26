import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_NUMBER, CONF_OUTPUT_ID
from . import M5Stack8Angle, m5stack_8angle_ns

DEPENDENCIES = ["m5stack_8angle"]
CONF_PARENT = "m5stack_8angle_id"
M5Stack8AngleLight = m5stack_8angle_ns.class_("M5Stack8AngleLight", light.LightOutput)

CONFIG_SCHEMA = light.RGB_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(M5Stack8AngleLight),
    cv.Required(CONF_PARENT): cv.use_id(M5Stack8Angle),
    cv.Required(CONF_NUMBER): cv.int_range(min=0, max=8),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_PARENT])
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID], parent, config[CONF_NUMBER])
    await light.register_light(var, config)
