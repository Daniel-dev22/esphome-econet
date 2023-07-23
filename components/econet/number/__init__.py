"""
Number component for Econet
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
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

EconetNumber = econet_ns.class_(
    "EconetNumber", cg.PollingComponent, EconetClient
)

CONF_ECONET_ID = "econet"

CONF_CC_DHUMSETP = "cc_dhumsetp"
CONF_MIN_VALUE = 10
CONF_MAX_VALUE = 80
CONF_STEP = 1

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(EconetNumber),
            cv.Optional(CONF_CC_DHUMSETP): number.NUMBER_SCHEMA.extend({
                cv.Optional(CONF_MAX_VALUE): cv.float_,
                cv.Optional(CONF_MIN_VALUE): cv.float_,
                cv.Optional(CONF_STEP): cv.positive_float,
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
        sens = await number.new_number(config[CONF_CC_DHUMSETP], min_value=10, max_value=80, step=1))
        cg.add(var.set_cc_dhumsetp_number(sens))
