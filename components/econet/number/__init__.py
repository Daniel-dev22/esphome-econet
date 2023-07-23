"""
Number component for Econet
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

CONF_CC_DHUMSETP = "cc_dhumsetp"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(EconetNumber),
            cv.Optional(CONF_CC_DHUMSETP): number.NUMBER_SCHEMA.extend({
                cv.Optional(CONF_MAX_VALUE, default=10): cv.float_,
                cv.Optional(CONF_MIN_VALUE, default=-80): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.positive_float,
            }),
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

    if CONF_CC_DHUMSETP in config:
        sens = await number.new_number(config[CONF_CC_DHUMSETP])
        cg.add(var.set_cc_dhumsetp_number(sens))
