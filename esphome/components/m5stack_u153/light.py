import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_NUMBER, CONF_OUTPUT_ID
from . import M5StackU153, m5stack_u153_ns

DEPENDENCIES = ["m5stack_u153"]
CONF_M5STACK_U153_ID = "m5stack_u153_id"

M5StackU153Light = m5stack_u153_ns.class_(
    "M5StackU153Light", light.LightOutput
)

CONFIG_SCHEMA = light.RGB_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(M5StackU153Light),
    cv.Required(CONF_M5STACK_U153_ID): cv.use_id(M5StackU153),
    cv.Required(CONF_NUMBER): cv.int_range(min=0, max=7),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_M5STACK_U153_ID])
    var = cg.new_Pvariable(
        config[CONF_OUTPUT_ID], parent, config[CONF_NUMBER]
    )
    await light.register_light(var, config)
