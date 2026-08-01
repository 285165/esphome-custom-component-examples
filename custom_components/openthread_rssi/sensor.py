"""Sensor platform for the openthread_rssi external component."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_SIGNAL_STRENGTH,
    ENTITY_CATEGORY_DIAGNOSTIC,
    STATE_CLASS_MEASUREMENT,
    UNIT_DECIBEL_MILLIWATT,
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


def _rssi_sensor_schema():
    return sensor.sensor_schema(
        unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    )


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_OPENTHREAD_RSSI_ID): cv.use_id(OpenThreadRSSIComponent),
        cv.Optional(CONF_PARENT_AVG_RSSI): _rssi_sensor_schema(),
        cv.Optional(CONF_PARENT_LAST_RSSI): _rssi_sensor_schema(),
        cv.Optional(CONF_NEIGHBOR_BEST_RSSI): _rssi_sensor_schema(),
        cv.Optional(CONF_NEIGHBOR_AVG_RSSI): _rssi_sensor_schema(),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OPENTHREAD_RSSI_ID])

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
