import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID, ENTITY_CATEGORY_DIAGNOSTIC
from . import OpenThreadDiagnostics

CONF_ROLE = "role"
CONF_NETWORK_NAME = "network_name"
CONF_THREAD_VERSION = "thread_version"
CONF_EXT_ADDRESS = "ext_address"
CONF_PAN_ID = "pan_id"
CONF_PARTITION_ID = "partition_id"
CONF_LEADER_ROUTER_ID = "leader_router_id"
CONF_RLOC16 = "rloc16"
CONF_IP_ADDRESSES = "ip_addresses"
CONF_MESH_LOCAL_ADDRESS = "mesh_local_address"
CONF_LINK_LOCAL_ADDRESS = "link_local_address"


def diag_text_sensor_schema():
    return text_sensor.text_sensor_schema(entity_category=ENTITY_CATEGORY_DIAGNOSTIC)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThreadDiagnostics),
        cv.Optional(CONF_ROLE): diag_text_sensor_schema(),
        cv.Optional(CONF_NETWORK_NAME): diag_text_sensor_schema(),
        cv.Optional(CONF_THREAD_VERSION): diag_text_sensor_schema(),
        cv.Optional(CONF_EXT_ADDRESS): diag_text_sensor_schema(),
        cv.Optional(CONF_PAN_ID): diag_text_sensor_schema(),
        cv.Optional(CONF_PARTITION_ID): diag_text_sensor_schema(),
        cv.Optional(CONF_LEADER_ROUTER_ID): diag_text_sensor_schema(),
        cv.Optional(CONF_RLOC16): diag_text_sensor_schema(),
        cv.Optional(CONF_IP_ADDRESSES): diag_text_sensor_schema(),
        cv.Optional(CONF_MESH_LOCAL_ADDRESS): diag_text_sensor_schema(),
        cv.Optional(CONF_LINK_LOCAL_ADDRESS): diag_text_sensor_schema(),
    }
).extend(cv.polling_component_schema("30s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    text_sensors = {
        CONF_ROLE: var.set_role_text_sensor,
        CONF_NETWORK_NAME: var.set_network_name_text_sensor,
        CONF_THREAD_VERSION: var.set_thread_version_text_sensor,
        CONF_EXT_ADDRESS: var.set_ext_address_text_sensor,
        CONF_PAN_ID: var.set_pan_id_text_sensor,
        CONF_PARTITION_ID: var.set_partition_id_text_sensor,
        CONF_LEADER_ROUTER_ID: var.set_leader_router_id_text_sensor,
        CONF_RLOC16: var.set_rloc16_text_sensor,
        CONF_IP_ADDRESSES: var.set_ip_addresses_text_sensor,
        CONF_MESH_LOCAL_ADDRESS: var.set_mesh_local_address_text_sensor,
        CONF_LINK_LOCAL_ADDRESS: var.set_link_local_address_text_sensor,
    }

    for key, setter in text_sensors.items():
        if key in config:
            sens = await text_sensor.new_text_sensor(config[key])
            cg.add(setter(sens))
