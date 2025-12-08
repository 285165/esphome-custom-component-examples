from esphome import pins
import esphome.codegen as cg
from esphome.components import i2c
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_INPUT,
    CONF_INVERTED,
    CONF_MODE,
    CONF_NUMBER,
    CONF_OUTPUT,
)

AUTO_LOAD = ["gpio_expander"]
DEPENDENCIES = ["i2c"]
MULTI_CONF = True

m5_4in8out_ns = cg.esphome_ns.namespace("m5_4in8out")

M5Stack4In8OutComponent = m5_4in8out_ns.class_("M5Stack4In8OutComponent", cg.Component, i2c.I2CDevice)
M5Stack4In8OutGPIOPin = m5_4in8out_ns.class_("M5Stack4In8OutGPIOPin", cg.GPIOPin)

CONF_M5STACK4IN8OUT = "m54in8out"
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.declare_id(M5Stack4In8OutComponent),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x45))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)


def validate_mode(value):
    if not (value[CONF_INPUT] or value[CONF_OUTPUT]):
        raise cv.Invalid("Mode must be either input or output")
    if value[CONF_INPUT] and value[CONF_OUTPUT]:
        raise cv.Invalid("Mode must be either input or output")
    return value


M5Stack4In8Out_PIN_SCHEMA = pins.gpio_base_schema(
    M5Stack4In8OutGPIOPin,
    cv.int_range(min=0, max=12),
    modes=[CONF_INPUT, CONF_OUTPUT],
    mode_validator=validate_mode,
    invertible=True,
).extend(
    {
        cv.Required(CONF_M5STACK4IN8OUT): cv.use_id(M5Stack4In8OutComponent),
    }
)


@pins.PIN_SCHEMA_REGISTRY.register(CONF_M5STACK4IN8OUT, M5Stack4In8Out_PIN_SCHEMA)
async def pcf8574_pin_to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    parent = await cg.get_variable(config[CONF_M5STACK4IN8OUT])

    cg.add(var.set_parent(parent))

    num = config[CONF_NUMBER]
    cg.add(var.set_pin(num))
    cg.add(var.set_inverted(config[CONF_INVERTED]))
    cg.add(var.set_flags(pins.gpio_flags_expr(config[CONF_MODE])))
    return var