"""OpenThread RSSI external component for ESPHome.

Exposes Parent/Neighbor RSSI, Link Quality, Link Margin, error rates and
neighbor topology as ESPHome sensors so the values can be published to
Home Assistant.
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@285165"]

# This component only makes sense together with the built-in openthread stack.
DEPENDENCIES = ["openthread"]
AUTO_LOAD = ["sensor"]

openthread_rssi_ns = cg.esphome_ns.namespace("openthread_rssi")
OpenThreadRSSIComponent = openthread_rssi_ns.class_(
    "OpenThreadRSSIComponent", cg.PollingComponent
)

CONF_OPENTHREAD_RSSI_ID = "openthread_rssi_id"
CONF_NOISE_FLOOR = "noise_floor"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThreadRSSIComponent),
        # Noise floor (dBm) used to derive the parent Link Margin from RSSI.
        # OpenThread's default receiver sensitivity constant is -100 dBm.
        cv.Optional(CONF_NOISE_FLOOR, default=-100): cv.int_range(min=-127, max=0),
    }
).extend(cv.polling_component_schema("30s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_noise_floor(config[CONF_NOISE_FLOOR]))
