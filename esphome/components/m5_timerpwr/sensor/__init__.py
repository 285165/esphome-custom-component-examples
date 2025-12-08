import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, i2c, sensor
from esphome.const import (
    CONF_BATTERY_LEVEL,
    CONF_BATTERY_VOLTAGE,
    CONF_ID,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_VOLTAGE,
    ICON_BATTERY,
    ICON_FLASH,
    STATE_CLASS_MEASUREMENT,
    UNIT_PERCENT,
    UNIT_VOLT,
    UNIT_MILLIAMP,
)

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["binary_sensor"]

timerpwr_ns = cg.esphome_ns.namespace("m5_timerpwr")
M5_TIMERPWR = timerpwr_ns.class_(
    "M5_timerpwr", cg.PollingComponent, i2c.I2CDevice
)

CONF_CHARGING = "charging"
CONF_BATTERY_CURRENT = "battery_current"
CONF_USB_VOLTAGE = "usb_voltage"
CONF_USB_CURRENT = "usb_current"
CONF_GROVE_VOLTAGE = "grove_voltage"
CONF_GROVE_CURRENT = "grove_current"
ICON_CURRENT_DC = "mdi:current-dc"
ICON_BATTERY_CHARGING = "mdi:battery-charging"
ICON_BATTERY_VOLTAGE = "mdi:flash-triangle"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(M5_TIMERPWR),
            cv.Optional(CONF_BATTERY_LEVEL): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_BATTERY,
                state_class=STATE_CLASS_MEASUREMENT,
                icon=ICON_BATTERY,
            ),
            cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(
                icon=ICON_BATTERY_CHARGING,
            ),
            cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                icon=ICON_BATTERY_VOLTAGE,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_VOLTAGE
            ),
            cv.Optional(CONF_BATTERY_CURRENT): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIAMP,
                icon=ICON_CURRENT_DC,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_CURRENT
            ),

            cv.Optional(CONF_USB_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                icon=ICON_FLASH,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_VOLTAGE
            ),
            cv.Optional(CONF_USB_CURRENT): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIAMP,
                icon=ICON_CURRENT_DC,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_CURRENT
            ),

            cv.Optional(CONF_GROVE_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                icon=ICON_FLASH,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_VOLTAGE
            ),
            cv.Optional(CONF_GROVE_CURRENT): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIAMP,
                icon=ICON_CURRENT_DC,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_CURRENT
            ),
        }
    )
    .extend(cv.polling_component_schema("10s"))
    .extend(i2c.i2c_device_schema(0x56))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if conf := config.get(CONF_CHARGING):
        bsens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_charging(bsens))

    if conf := config.get(CONF_BATTERY_LEVEL):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_battery_level(sens))
    if conf := config.get(CONF_BATTERY_VOLTAGE):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_battery_voltage(sens))
    if conf := config.get(CONF_BATTERY_CURRENT):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_battery_current(sens))

    if conf := config.get(CONF_USB_VOLTAGE):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_usb_voltage(sens))
    if conf := config.get(CONF_USB_CURRENT):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_usb_current(sens))

    if conf := config.get(CONF_GROVE_VOLTAGE):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_grove_voltage(sens))
    if conf := config.get(CONF_GROVE_CURRENT):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_grove_current(sens))

