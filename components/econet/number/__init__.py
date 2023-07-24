"""
Number component for Econet
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import ( 
     CONF_ID, 
     CONF_INITIAL_VALUE, 
     CONF_LAMBDA, 
     CONF_MAX_VALUE, 
     CONF_MIN_VALUE, 
     CONF_OPTIMISTIC, 
     CONF_RESTORE_VALUE, 
     CONF_STEP, 
     CONF_MODE, 
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
# CONF_MIN_VALUE = min_value
# CONF_MAX_VALUE = max_value
# CONF_STEP = step
# CONF_MODE = "slider"

ECONET_NUMBER_SCHEMA = number.number_schema(EconetNumber).extend( 
    { 
        cv.Required(CONF_MAX_VALUE): cv.float_, 
        cv.Required(CONF_MIN_VALUE): cv.float_, 
        cv.Required(CONF_STEP): cv.positive_float,
    }
)

CONFIG_SCHEMA = ECONET_CLIENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CC_DHUMSETP): ECONET_NUMBER_SCHEMA,
        #cv.Optional(CONF_CC_SOMETHING): ECONET_NUMBER_SCHEMA,
    }
)






async def to_code(config): 
    var = await cg.get_variable(config[CONF_ECONET_ID])

    if CONF_CC_DHUMSETP in config:
      conf = config[CONF_CC_DHUMSETP]
      sens = await number.new_number(conf, min_value=conf[CONF_MIN_VALUE], max_value=conf[CONF_MAX_VALUE], step=conf[CONF_STEP])
      cg.add(var.set_cc_dhumsetp_number(sens))
