"""OpenThread RSSI external component for ESPHome.

Exposes Parent RSSI (for Child devices) and the best/average Neighbor RSSI
(for Router/Leader/FTD devices) as ESPHome sensors so the values can be
published to Home Assistant.
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

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThreadRSSIComponent),
    }
).extend(cv.polling_component_schema("30s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
