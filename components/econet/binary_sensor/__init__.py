"""
Binary Sensor component for Econet
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, DEVICE_CLASS_RUNNING

from .. import CONF_ECONET_ID, ECONET_CLIENT_SCHEMA, EconetClient, econet_ns

EconetBinarySensor = econet_ns.class_(
    "EconetBinarySensor", cg.PollingComponent, EconetClient
)

CONF_ENABLE_STATE = "enable_state"
CONF_HEATCTRL = "heatctrl"
CONF_FAN_CTRL = "fan_ctrl"
CONF_COMP_RLY = "comp_rly"

BINARY_SENSORS = {
    CONF_ENABLE_STATE: "WHTRENAB",
    CONF_HEATCTRL: "HEATCTRL",
    CONF_FAN_CTRL: "FAN_CTRL",
    CONF_COMP_RLY: "COMP_RLY",
}

CONFIG_SCHEMA = (
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(EconetBinarySensor),
            cv.Optional(CONF_ENABLE_STATE): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_RUNNING,
            ),
        },
        {
            cv.GenerateID(): cv.declare_id(EconetBinarySensor),
            cv.Optional(CONF_HEATCTRL): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_RUNNING,
            ),
        },
        {
            cv.GenerateID(): cv.declare_id(EconetBinarySensor),
            cv.Optional(CONF_FAN_CTRL): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_RUNNING,
            ),
        },
        {
            cv.GenerateID(): cv.declare_id(EconetBinarySensor),
            cv.Optional(CONF_COMP_RLY): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_RUNNING,
            ),
        },
    )
    .extend(ECONET_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("1s"))
)


async def to_code(config):
    """Generate main.cpp code"""
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    econet_var = await cg.get_variable(config[CONF_ECONET_ID])
    cg.add(var.set_econet(econet_var))

    for config_key, obg_key in BINARY_SENSORS.items():
        if config_key in config:
            sens = await binary_sensor.new_binary_sensor(config[config_key])
            cg.add(var.set_binary_sensor(obg_key, sens))
