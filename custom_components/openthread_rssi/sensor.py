"""Sensor platform for the openthread_rssi external component."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.core import Device
from esphome.const import (
    CONF_DEVICE_ID,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    ENTITY_CATEGORY_DIAGNOSTIC,
    STATE_CLASS_MEASUREMENT,
    UNIT_DECIBEL,
    UNIT_DECIBEL_MILLIWATT,
    UNIT_EMPTY,
    UNIT_PERCENT,
    UNIT_SECOND,
)

from . import (
    CONF_OPENTHREAD_RSSI_ID,
    OpenThreadRSSIComponent,
    openthread_rssi_ns,  # noqa: F401
)

DEPENDENCIES = ["openthread_rssi"]

CONF_PARENT_AVG_RSSI = "parent_avg_rssi"
CONF_PARENT_LAST_RSSI = "parent_last_rssi"
CONF_NEIGHBOR_BEST_RSSI = "neighbor_best_rssi"
CONF_NEIGHBOR_AVG_RSSI = "neighbor_avg_rssi"

# Link Quality (LQI, 0-3) and Link Margin (dB)
CONF_PARENT_LINK_QUALITY_IN = "parent_link_quality_in"
CONF_PARENT_LINK_QUALITY_OUT = "parent_link_quality_out"
CONF_PARENT_LINK_MARGIN = "parent_link_margin"
CONF_NEIGHBOR_BEST_LINK_QUALITY = "neighbor_best_link_quality"
CONF_NEIGHBOR_BEST_LINK_MARGIN = "neighbor_best_link_margin"

# Error rates (%) from the best-RSSI neighbor
CONF_NEIGHBOR_BEST_FRAME_ERROR_RATE = "neighbor_best_frame_error_rate"
CONF_NEIGHBOR_BEST_MESSAGE_ERROR_RATE = "neighbor_best_message_error_rate"

# Topology / freshness
CONF_NEIGHBOR_COUNT = "neighbor_count"
CONF_NEIGHBOR_BEST_AGE = "neighbor_best_age"


def _rssi_sensor_schema():
    return sensor.sensor_schema(
        unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    )


def _link_quality_sensor_schema():
    # LQI is a 0..3 quality index (no unit, no device_class)
    return sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    )


def _link_margin_sensor_schema():
    # Link Margin is a dB value above the noise floor
    return sensor.sensor_schema(
        unit_of_measurement=UNIT_DECIBEL,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    )


def _error_rate_sensor_schema():
    # Frame / message error rate as a percentage (0-100)
    return sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=1,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    )


def _count_sensor_schema():
    # Plain count of neighbors (unit-less integer)
    return sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    )


def _age_sensor_schema():
    # Seconds since the neighbor was last heard
    return sensor.sensor_schema(
        unit_of_measurement=UNIT_SECOND,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_DURATION,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    )


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_OPENTHREAD_RSSI_ID): cv.use_id(OpenThreadRSSIComponent),
        # Platform-level sub-device assignment (like internal_temperature's
        # per-entity device_id). When set, it is applied to every sensor below
        # that does not define its own device_id.
        cv.Optional(CONF_DEVICE_ID): cv.use_id(Device),
        cv.Optional(CONF_PARENT_AVG_RSSI): _rssi_sensor_schema(),
        cv.Optional(CONF_PARENT_LAST_RSSI): _rssi_sensor_schema(),
        cv.Optional(CONF_NEIGHBOR_BEST_RSSI): _rssi_sensor_schema(),
        cv.Optional(CONF_NEIGHBOR_AVG_RSSI): _rssi_sensor_schema(),
        cv.Optional(CONF_PARENT_LINK_QUALITY_IN): _link_quality_sensor_schema(),
        cv.Optional(CONF_PARENT_LINK_QUALITY_OUT): _link_quality_sensor_schema(),
        cv.Optional(CONF_PARENT_LINK_MARGIN): _link_margin_sensor_schema(),
        cv.Optional(CONF_NEIGHBOR_BEST_LINK_QUALITY): _link_quality_sensor_schema(),
        cv.Optional(CONF_NEIGHBOR_BEST_LINK_MARGIN): _link_margin_sensor_schema(),
        cv.Optional(CONF_NEIGHBOR_BEST_FRAME_ERROR_RATE): _error_rate_sensor_schema(),
        cv.Optional(CONF_NEIGHBOR_BEST_MESSAGE_ERROR_RATE): _error_rate_sensor_schema(),
        cv.Optional(CONF_NEIGHBOR_COUNT): _count_sensor_schema(),
        cv.Optional(CONF_NEIGHBOR_BEST_AGE): _age_sensor_schema(),
    }
)

# Keys that hold a child sensor configuration (used to propagate device_id).
_SENSOR_KEYS = [
    CONF_PARENT_AVG_RSSI,
    CONF_PARENT_LAST_RSSI,
    CONF_NEIGHBOR_BEST_RSSI,
    CONF_NEIGHBOR_AVG_RSSI,
    CONF_PARENT_LINK_QUALITY_IN,
    CONF_PARENT_LINK_QUALITY_OUT,
    CONF_PARENT_LINK_MARGIN,
    CONF_NEIGHBOR_BEST_LINK_QUALITY,
    CONF_NEIGHBOR_BEST_LINK_MARGIN,
    CONF_NEIGHBOR_BEST_FRAME_ERROR_RATE,
    CONF_NEIGHBOR_BEST_MESSAGE_ERROR_RATE,
    CONF_NEIGHBOR_COUNT,
    CONF_NEIGHBOR_BEST_AGE,
]


def _propagate_device_id(config):
    """Apply the platform-level device_id to every child sensor that does not
    already define its own, mirroring the per-entity device_id behavior."""
    device_id = config.get(CONF_DEVICE_ID)
    if device_id is None:
        return
    for key in _SENSOR_KEYS:
        if key in config and CONF_DEVICE_ID not in config[key]:
            config[key][CONF_DEVICE_ID] = device_id


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OPENTHREAD_RSSI_ID])

    # Fan the platform-level device_id out to each sub-sensor before creation.
    _propagate_device_id(config)

    if CONF_PARENT_AVG_RSSI in config:
        sens = await sensor.new_sensor(config[CONF_PARENT_AVG_RSSI])
        cg.add(parent.set_parent_avg_rssi_sensor(sens))
    if CONF_PARENT_LAST_RSSI in config:
        sens = await sensor.new_sensor(config[CONF_PARENT_LAST_RSSI])
        cg.add(parent.set_parent_last_rssi_sensor(sens))
    if CONF_NEIGHBOR_BEST_RSSI in config:
        sens = await sensor.new_sensor(config[CONF_NEIGHBOR_BEST_RSSI])
        cg.add(parent.set_neighbor_best_rssi_sensor(sens))
    if CONF_NEIGHBOR_AVG_RSSI in config:
        sens = await sensor.new_sensor(config[CONF_NEIGHBOR_AVG_RSSI])
        cg.add(parent.set_neighbor_avg_rssi_sensor(sens))

    if CONF_PARENT_LINK_QUALITY_IN in config:
        sens = await sensor.new_sensor(config[CONF_PARENT_LINK_QUALITY_IN])
        cg.add(parent.set_parent_link_quality_in_sensor(sens))
    if CONF_PARENT_LINK_QUALITY_OUT in config:
        sens = await sensor.new_sensor(config[CONF_PARENT_LINK_QUALITY_OUT])
        cg.add(parent.set_parent_link_quality_out_sensor(sens))
    if CONF_PARENT_LINK_MARGIN in config:
        sens = await sensor.new_sensor(config[CONF_PARENT_LINK_MARGIN])
        cg.add(parent.set_parent_link_margin_sensor(sens))
    if CONF_NEIGHBOR_BEST_LINK_QUALITY in config:
        sens = await sensor.new_sensor(config[CONF_NEIGHBOR_BEST_LINK_QUALITY])
        cg.add(parent.set_neighbor_best_link_quality_sensor(sens))
    if CONF_NEIGHBOR_BEST_LINK_MARGIN in config:
        sens = await sensor.new_sensor(config[CONF_NEIGHBOR_BEST_LINK_MARGIN])
        cg.add(parent.set_neighbor_best_link_margin_sensor(sens))
    if CONF_NEIGHBOR_BEST_FRAME_ERROR_RATE in config:
        sens = await sensor.new_sensor(config[CONF_NEIGHBOR_BEST_FRAME_ERROR_RATE])
        cg.add(parent.set_neighbor_best_frame_error_rate_sensor(sens))
    if CONF_NEIGHBOR_BEST_MESSAGE_ERROR_RATE in config:
        sens = await sensor.new_sensor(config[CONF_NEIGHBOR_BEST_MESSAGE_ERROR_RATE])
        cg.add(parent.set_neighbor_best_message_error_rate_sensor(sens))
    if CONF_NEIGHBOR_COUNT in config:
        sens = await sensor.new_sensor(config[CONF_NEIGHBOR_COUNT])
        cg.add(parent.set_neighbor_count_sensor(sens))
    if CONF_NEIGHBOR_BEST_AGE in config:
        sens = await sensor.new_sensor(config[CONF_NEIGHBOR_BEST_AGE])
        cg.add(parent.set_neighbor_best_age_sensor(sens))
