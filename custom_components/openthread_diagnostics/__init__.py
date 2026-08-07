import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, text_sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    ENTITY_CATEGORY_DIAGNOSTIC,
    UNIT_DECIBEL,
    UNIT_PERCENT,
)

DEPENDENCIES = ["openthread"]
AUTO_LOAD = ["sensor", "text_sensor"]

openthread_diagnostics_ns = cg.esphome_ns.namespace("openthread_diagnostics")
OpenThreadDiagnostics = openthread_diagnostics_ns.class_(
    "OpenThreadDiagnostics", cg.PollingComponent
)

CONF_ROLE = "role"
CONF_NETWORK_NAME = "network_name"
CONF_EXT_ADDRESS = "extended_address"
CONF_MESH_LOCAL_ADDRESS = "mesh_local_address"
CONF_LINK_LOCAL_ADDRESS = "link_local_address"
CONF_IP_ADDRESSES = "ip_addresses"
CONF_THREAD_VERSION = "thread_version"

CONF_CHANNEL = "channel"
CONF_PAN_ID = "pan_id"
CONF_RLOC16 = "rloc16"
CONF_PARTITION_ID = "partition_id"
CONF_LEADER_ROUTER_ID = "leader_router_id"
CONF_TX_POWER = "tx_power"
CONF_NEIGHBOR_COUNT = "neighbor_count"
CONF_CHILD_COUNT = "child_count"
CONF_PARENT_RSSI = "parent_rssi"
CONF_PARENT_AVERAGE_RSSI = "parent_average_rssi"
CONF_PARENT_LINK_MARGIN = "parent_link_margin"
CONF_PARENT_LINK_QUALITY = "parent_link_quality"
CONF_PARENT_AGE = "parent_age"
CONF_FRAME_ERROR_RATE = "frame_error_rate"
CONF_MESSAGE_ERROR_RATE = "message_error_rate"
CONF_MAC_TX_TOTAL = "mac_tx_total"
CONF_MAC_RX_TOTAL = "mac_rx_total"
CONF_MAC_TX_ERR_CCA = "mac_tx_err_cca"
CONF_MAC_TX_RETRY = "mac_tx_retry"

SENSOR_KEYS = {
    CONF_CHANNEL: "set_channel_sensor",
    CONF_PAN_ID: "set_pan_id_sensor",
    CONF_RLOC16: "set_rloc16_sensor",
    CONF_PARTITION_ID: "set_partition_id_sensor",
    CONF_LEADER_ROUTER_ID: "set_leader_router_id_sensor",
    CONF_TX_POWER: "set_tx_power_sensor",
    CONF_NEIGHBOR_COUNT: "set_neighbor_count_sensor",
    CONF_CHILD_COUNT: "set_child_count_sensor",
    CONF_PARENT_RSSI: "set_parent_rssi_sensor",
    CONF_PARENT_AVERAGE_RSSI: "set_parent_average_rssi_sensor",
    CONF_PARENT_LINK_MARGIN: "set_parent_link_margin_sensor",
    CONF_PARENT_LINK_QUALITY: "set_parent_link_quality_sensor",
    CONF_PARENT_AGE: "set_parent_age_sensor",
    CONF_FRAME_ERROR_RATE: "set_frame_error_rate_sensor",
    CONF_MESSAGE_ERROR_RATE: "set_message_error_rate_sensor",
    CONF_MAC_TX_TOTAL: "set_mac_tx_total_sensor",
    CONF_MAC_RX_TOTAL: "set_mac_rx_total_sensor",
    CONF_MAC_TX_ERR_CCA: "set_mac_tx_err_cca_sensor",
    CONF_MAC_TX_RETRY: "set_mac_tx_retry_sensor",
}

TEXT_SENSOR_KEYS = {
    CONF_ROLE: "set_role_text_sensor",
    CONF_NETWORK_NAME: "set_network_name_text_sensor",
    CONF_EXT_ADDRESS: "set_ext_address_text_sensor",
    CONF_MESH_LOCAL_ADDRESS: "set_mesh_local_address_text_sensor",
    CONF_LINK_LOCAL_ADDRESS: "set_link_local_address_text_sensor",
    CONF_IP_ADDRESSES: "set_ip_addresses_text_sensor",
    CONF_THREAD_VERSION: "set_thread_version_text_sensor",
}


def diag_sensor_schema(**kw**gs):
    kwargs.setdefault("accur**y_decimals", 0)
    return sensor**ensor_schema(
        entity_cate**ry=ENTITY_CATEGORY_DIAGNOSTIC,
  **    **kwargs,
    )

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThreadDiagnostics),
        cv.Optional(CONF_CHANNEL): diag_sensor_schema(),
        cv.Optional(CONF_PAN_ID): diag_sensor_schema(),
        cv.Optional(CONF_RLOC16): diag_sensor_schema(),
        cv.Optional(CONF_PARTITION_ID): diag_sensor_schema(),
        cv.Optional(CONF_LEADER_ROUTER_ID): diag_sensor_schema(),
        cv.Optional(CONF_TX_POWER): diag_sensor_schema(unit_of_measurement="dBm"),
        cv.Optional(CONF_NEIGHBOR_COUNT): diag_sensor_schema(),
        cv.Optional(CONF_CHILD_COUNT): diag_sensor_schema(),
        cv.Optional(CONF_PARENT_RSSI): diag_sensor_schema(
            unit_of_measurement=UNIT_DECIBEL,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
        ),
        cv.Optional(CONF_PARENT_AVERAGE_RSSI): diag_sensor_schema(
            unit_of_measurement=UNIT_DECIBEL,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
        ),
        cv.Optional(CONF_PARENT_LINK_MARGIN): diag_sensor_schema(unit_of_measurement="dB"),
        cv.Optional(CONF_PARENT_LINK_QUALITY): diag_sensor_schema(),
        cv.Optional(CONF_PARENT_AGE): diag_sensor_schema(unit_of_measurement="s"),
        cv.Optional(CONF_FRAME_ERROR_RATE): diag_sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=2,
        ),
        cv.Optional(CONF_MESSAGE_ERROR_RATE): diag_sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=2,
        ),
        cv.Optional(CONF_MAC_TX_TOTAL): diag_sensor_schema(),
        cv.Optional(CONF_MAC_RX_TOTAL): diag_sensor_schema(),
        cv.Optional(CONF_MAC_TX_ERR_CCA): diag_sensor_schema(),
        cv.Optional(CONF_MAC_TX_RETRY): diag_sensor_schema(),
        cv.Optional(CONF_ROLE): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_NETWORK_NAME): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_EXT_ADDRESS): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_MESH_LOCAL_ADDRESS): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_LINK_LOCAL_ADDRESS): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_IP_ADDRESSES): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_THREAD_VERSION): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
    }
).extend(cv.polling_component_schema("30s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for key, setter in SENSOR_KEYS.items():
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(getattr(var, setter)(sens))

    for key, setter in TEXT_SENSOR_KEYS.items():
        if key in config:
            sens = await text_sensor.new_text_sensor(config[key])
            cg.add(getattr(var, setter)(sens))
