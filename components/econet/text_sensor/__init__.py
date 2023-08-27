"""
Text Sensor component for Econet
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    CONF_ID,
    UNIT_CELSIUS,
    ICON_THERMOMETER,
    DEVICE_CLASS_SPEED,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
    STATE_CLASS_MEASUREMENT,
)

from .. import (
    econet_ns,
    CONF_ECONET_ID,
    ECONET_CLIENT_SCHEMA,
    EconetClient,
)

EconetTextSensor = econet_ns.class_(
    "EconetTextSensor", cg.PollingComponent, EconetClient
)

CONF_ECONET_ID = "econet"

CONF_CC_HVACMODE_TEXT = "cc_hvacmode_text"
CONF_CC_AUTOMODE_TEXT = "cc_automode_text"
CONF_WATER_HEATER_FAN_SPEED = "water_heater_fan_speed"

TEXT_SENSORS = [
    CONF_WATER_HEATER_FAN_SPEED,
    CONF_CC_HVACMODE_TEXT,
    CONF_CC_AUTOMODE_TEXT
]

CONFIG_SCHEMA = (
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(EconetTextSensor),
            cv.Optional(CONF_WATER_HEATER_FAN_SPEED): text_sensor.text_sensor_schema(

            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetTextSensor),
            cv.Optional(CONF_CC_HVACMODE_TEXT): text_sensor.text_sensor_schema(

            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetTextSensor),
            cv.Optional(CONF_CC_AUTOMODE_TEXT): text_sensor.text_sensor_schema(

            )
        }
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


    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(var,f"set_{key}_text_sensor")(sens))