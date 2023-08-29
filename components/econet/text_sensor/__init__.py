"""
Text Sensor component for Econet
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID

from .. import CONF_ECONET_ID, ECONET_CLIENT_SCHEMA, EconetClient, econet_ns

EconetTextSensor = econet_ns.class_(
    "EconetTextSensor", cg.PollingComponent, EconetClient
)

CONF_CC_HVACMODE_TEXT = "cc_hvacmode_text"
CONF_CC_AUTOMODE_TEXT = "cc_automode_text"
CONF_WATER_HEATER_FAN_SPEED = "water_heater_fan_speed"
CONF_WATER_HEATER_HEATING_ELEMENT_STATE = "water_heater_heating_element_state"

TEXT_SENSORS = {
    CONF_CC_HVACMODE_TEXT: "HVACMODE",
    CONF_CC_AUTOMODE_TEXT: "AUTOMODE",
    CONF_WATER_HEATER_FAN_SPEED: "FAN_CTRL",
    CONF_WATER_HEATER_HEATING_ELEMENT_STATE: "HEATCTRL",
}

CONFIG_SCHEMA = (
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(EconetTextSensor),
            cv.Optional(CONF_CC_HVACMODE_TEXT): text_sensor.text_sensor_schema(),
        },
        {
            cv.GenerateID(): cv.declare_id(EconetTextSensor),
            cv.Optional(CONF_CC_AUTOMODE_TEXT): text_sensor.text_sensor_schema(),
        },
        {
            cv.GenerateID(): cv.declare_id(EconetTextSensor),
            cv.Optional(CONF_WATER_HEATER_FAN_SPEED): text_sensor.text_sensor_schema(),
        },
        {
            cv.GenerateID(): cv.declare_id(EconetTextSensor),
            cv.Optional(CONF_WATER_HEATER_HEATING_ELEMENT_STATE): text_sensor.text_sensor_schema(),
        },
    )
    .extend(ECONET_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("5s"))
)


async def to_code(config):
    """Generate main.cpp code"""

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    econet_var = await cg.get_variable(config[CONF_ECONET_ID])
    cg.add(var.set_econet(econet_var))

    for config_key, obg_key in TEXT_SENSORS.items():
        if config_key in config:
            sens = await text_sensor.new_text_sensor(config[config_key])
            cg.add(var.set_text_sensor(obg_key, sens))
