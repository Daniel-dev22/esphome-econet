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

#CONFIG_SCHEMA = cv.All( 
#    number.number_schema(EconetNumber) 
#    .extend( 
#        { 
#            cv.Optional(CONF_CC_DHUMSETP),
#            cv.Required(CONF_MAX_VALUE): cv.float_, 
#            cv.Required(CONF_MIN_VALUE): cv.float_, 
#            cv.Required(CONF_STEP): cv.positive_float,
#         }
#     )
#        .extend(ECONET_CLIENT_SCHEMA)
#        .extend(cv.polling_component_schema("5s")), 
# )

ECONET_NUMBER_SCHEMA = number.NUMBER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(EconetNumber),
        cv.Optional(CONF_STEP, default=0.01): cv.float_,
        cv.Optional(CONF_MODE, default="SLIDER"): cv.enum(number.NUMBER_MODES, upper=True),
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = ECONET_CLIENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CC_DHUMSETP): ECONET_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=10): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=80): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
    }
)




async def to_code(config): 
    var = cg.new_Pvariable("DHUMSETPOINT")
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

