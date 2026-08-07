import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    ENTITY_CATEGORY_DIAGNOSTIC,
    UNIT_DECIBEL,
    UNIT_DECIBEL_MILLIWATT,
    UNIT_PERCENT,
)
from . import OpenThreadDiagnostics

CONF_CHANNEL = "channel"
CONF_TX_POWER = "tx_power"
CONF_PARENT_RSSI = "parent_rssi"
CONF_PARENT_AVERAGE_RSSI = "parent_average_rssi"
CONF_PARENT_LINK_MARGIN = "parent_link_margin"
CONF_PARENT_LINK_QUALITY = "parent_link_quality"
CONF_PARENT_AGE = "parent_age"
CONF_FRAME_ERROR_RATE = "frame_error_rate"
CONF_MESSAGE_ERROR_RATE = "message_error_rate"
CONF_NEIGHBOR_COUNT = "neighbor_count"
CONF_CHILD_COUNT = "child_count"
CONF_MAC_TX_TOTAL = "mac_tx_total"
CONF_MAC_RX_TOTAL = "mac_rx_total"
CONF_MAC_TX_ERR_CCA = "mac_tx_err_cca"
CONF_MAC_TX_RETRY = "mac_tx_retry"


def diag_sensor_schema(**kwargs):
    kwargs.setdefault("accuracy_decimals", 0)
    return sensor.sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        **kwargs,
    )


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThreadDiagnostics),
        cv.Optional(CONF_CHANNEL): diag_sensor_schema(),
        cv.Optional(CONF_TX_POWER): diag_sensor_schema(
            unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
        ),
        cv.Optional(CONF_PARENT_RSSI): diag_sensor_schema(
            unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
        ),
        cv.Optional(CONF_PARENT_AVERAGE_RSSI): diag_sensor_schema(
            unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
        ),
        cv.Optional(CONF_PARENT_LINK_MARGIN): diag_sensor_schema(unit_of_measurement=UNIT_DECIBEL),
        cv.Optional(CONF_PARENT_LINK_QUALITY): diag_sensor_schema(),
        cv.Optional(CONF_PARENT_AGE): diag_sensor_schema(),
        cv.Optional(CONF_FRAME_ERROR_RATE): diag_sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=2,
        ),
        cv.Optional(CONF_MESSAGE_ERROR_RATE): diag_sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=2,
        ),
        cv.Optional(CONF_NEIGHBOR_COUNT): diag_sensor_schema(),
        cv.Optional(CONF_CHILD_COUNT): diag_sensor_schema(),
        cv.Optional(CONF_MAC_TX_TOTAL): diag_sensor_schema(),
        cv.Optional(CONF_MAC_RX_TOTAL): diag_sensor_schema(),
        cv.Optional(CONF_MAC_TX_ERR_CCA): diag_sensor_schema(),
        cv.Optional(CONF_MAC_TX_RETRY): diag_sensor_schema(),
    }
).extend(cv.polling_component_schema("30s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    sensors = {
        CONF_CHANNEL: var.set_channel_sensor,
        CONF_TX_POWER: var.set_tx_power_sensor,
        CONF_PARENT_RSSI: var.set_parent_rssi_sensor,
        CONF_PARENT_AVERAGE_RSSI: var.set_parent_average_rssi_sensor,
        CONF_PARENT_LINK_MARGIN: var.set_parent_link_margin_sensor,
        CONF_PARENT_LINK_QUALITY: var.set_parent_link_quality_sensor,
        CONF_PARENT_AGE: var.set_parent_age_sensor,
        CONF_FRAME_ERROR_RATE: var.set_frame_error_rate_sensor,
        CONF_MESSAGE_ERROR_RATE: var.set_message_error_rate_sensor,
        CONF_NEIGHBOR_COUNT: var.set_neighbor_count_sensor,
        CONF_CHILD_COUNT: var.set_child_count_sensor,
        CONF_MAC_TX_TOTAL: var.set_mac_tx_total_sensor,
        CONF_MAC_RX_TOTAL: var.set_mac_rx_total_sensor,
        CONF_MAC_TX_ERR_CCA: var.set_mac_tx_err_cca_sensor,
        CONF_MAC_TX_RETRY: var.set_mac_tx_retry_sensor,
    }

    for key, setter in sensors.items():
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(setter(sens))
