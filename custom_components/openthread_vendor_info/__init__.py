import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["openthread"]
AUTO_LOAD = []

CONF_VENDOR_NAME = "vendor_name"
CONF_VENDOR_MODEL = "vendor_model"
CONF_VENDOR_SW_VERSION = "vendor_sw_version"
CONF_VENDOR_APP_URL = "vendor_app_url"
CONF_TX_POWER_DBM = "tx_power_dbm"
CONF_SET_ON_EVERY_RETRY = "set_on_every_retry"

openthread_vendor_info_ns = cg.esphome_ns.namespace("openthread_vendor_info")
OpenThreadVendorInfoComponent = openthread_vendor_info_ns.class_(
    "OpenThreadVendorInfoComponent", cg.PollingComponent
)


def _ot_vendor_string(value):
    value = cv.string_strict(value)
    if len(value.encode("utf-8")) > 32:
        raise cv.Invalid("OpenThread vendor diagnostic string must be <= 32 bytes UTF-8")
    return value


def _ot_vendor_name(value):
    value = _ot_vendor_string(value)
    # ESP-IDF/OpenThread reference-device builds require the RD: prefix.
    if not value.startswith("RD:"):
        value = "RD:" + value
    if len(value.encode("utf-8")) > 32:
        raise cv.Invalid("OpenThread vendor_name with RD: prefix must be <= 32 bytes UTF-8")
    return value


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThreadVendorInfoComponent),
        cv.Optional(CONF_VENDOR_NAME): _ot_vendor_name,
        cv.Optional(CONF_VENDOR_MODEL): _ot_vendor_string,
        cv.Optional(CONF_VENDOR_SW_VERSION): _ot_vendor_string,
        cv.Optional(CONF_VENDOR_APP_URL): cv.string_strict,
        # OpenThread transmit power in dBm (platform radio API, int8_t).
        cv.Optional(CONF_TX_POWER_DBM): cv.int_range(min=-128, max=127),
        cv.Optional(CONF_SET_ON_EVERY_RETRY, default=False): cv.boolean,
    }
).extend(cv.polling_component_schema("30s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_VENDOR_NAME in config:
        cg.add(var.set_vendor_name(config[CONF_VENDOR_NAME]))
    if CONF_VENDOR_MODEL in config:
        cg.add(var.set_vendor_model(config[CONF_VENDOR_MODEL]))
    if CONF_VENDOR_SW_VERSION in config:
        cg.add(var.set_vendor_sw_version(config[CONF_VENDOR_SW_VERSION]))
    if CONF_VENDOR_APP_URL in config:
        cg.add(var.set_vendor_app_url(config[CONF_VENDOR_APP_URL]))
    if CONF_TX_POWER_DBM in config:
        cg.add(var.set_tx_power_dbm(config[CONF_TX_POWER_DBM]))

    cg.add(var.set_set_on_every_retry(config[CONF_SET_ON_EVERY_RETRY]))