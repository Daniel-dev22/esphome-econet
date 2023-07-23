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
CONF_MODE = "slider"

CONFIG_SCHEMA = cv.All( 
    number.number_schema(EconetNumber) 
    .extend( 
        { 
            cv.Required(CONF_MAX_VALUE): cv.float_, 
            cv.Required(CONF_MIN_VALUE): cv.float_, 
            cv.Required(CONF_STEP): cv.positive_float,
         }
     )
        .extend(ECONET_CLIENT_SCHEMA)
        .extend(cv.polling_component_schema("5s")), 
 )



async def to_code(config): 
    var = cg.new_Pvariable(config[CONF_ID]) 
    await cg.register_component(var, config) 
    await number.register_number( 
        var, 
        config, 
        min_value=config[CONF_MIN_VALUE], 
        max_value=config[CONF_MAX_VALUE], 
        step=config[CONF_STEP], 
        mode=config[CONF_MODE],
     )
    econet_var = await cg.get_variable(config[CONF_ECONET_ID])
    cg.add(var.set_econet(econet_var))

    if CONF_CC_DHUMSETP in config:
      sens = await number.new_number(config[CONF_CC_DHUMSETP], min_value=10, max_value=80, step=1)
    cg.add(var.set_cc_dhumsetp_number(sens))

