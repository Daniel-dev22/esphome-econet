"""
Switch component for Econet
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import (
    CONF_ID,
	CONF_SWITCH_DATAPOINT
)

from .. import (
    econet_ns,
	Econet,
    CONF_ECONET_ID,
    ECONET_CLIENT_SCHEMA,
    EconetClient,
)

DEPENDENCIES = ['econet']

EconetSwitch = econet_ns.class_(
    "EconetSwitch", switch.Switch, cg.PollingComponent, EconetClient
)

CONF_ENABLE_SWITCH = "enable_switch"
CONF_CC_DHUM_ENABLE_STATE = "cc_dhum_enable_state"
CONF_DUMMY_SWITCH = "dummy_switch"

SWITCHES = [
    CONF_ENABLE_SWITCH,
    CONF_CC_DHUM_ENABLE_STATE
]


ECONET_SWITCH_SCHEMA = switch.switch_schema(EconetSwitch).extend( 
    { 
        cv.Optional(CONF_SWITCH_DATAPOINT): cv.uint8_t, 
        
    }
)

CONFIG_SCHEMA = (
    ECONET_CLIENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(EconetSwitch),
            cv.Optional(CONF_ENABLE_SWITCH): ECONET_SWITCH_SCHEMA,
            cv.Optional(CONF_CC_DHUM_ENABLE_STATE): ECONET_SWITCH_SCHEMA,
        }
    )
    .extend(cv.COMPONENT_SCHEMA).extend(cv.polling_component_schema("5s"))
)

async def to_code(config): 

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    econet_var = await cg.get_variable(config[CONF_ECONET_ID])
    cg.add(var.set_econet(econet_var))


    for key in SWITCHES:
        if key in config:
            conf = config[key]
            sens = await switch.new_switch(conf)
            cg.add(getattr(var,f"set_{key}_switch")(sens))