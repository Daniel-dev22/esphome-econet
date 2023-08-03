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

EconetSwitch = econet_ns.class_(
    "EconetSwitch", switch.Switch, cg.PollingComponent, EconetClient
)

CONF_ENABLE_SWITCH = "enable_switch"
CONF_CC_DHUM_ENABLE_STATE = "cc_dhum_enable_state"
CONF_DUMMY_SWITCH = "dummy_switch"

SWITCHES = [
    CONF_CC_DHUM_ENABLE_STATE,
    CONF_ENABLE_SWITCH,
]

#CONFIG_SCHEMA = (
   # switch.switch_schema(EconetSwitch)
  #  .extend(
   #     {
  #          cv.GenerateID(CONF_ECONET_ID): cv.use_id(Econet),
  #          cv.Optional(CONF_SWITCH_DATAPOINT): cv.uint8_t,
   #     },
  #      {
  #          cv.GenerateID(CONF_ECONET_ID): cv.use_id(Econet),
   #         cv.Optional(CONF_CC_DHUM_ENABLE_STATE): cv.string,
  #      }
   # )
  #  .extend(cv.COMPONENT_SCHEMA).extend(cv.polling_component_schema("1s"))
#)

ECONET_SWITCH_SCHEMA = switch.switch_schema(EconetSwitch).extend( 
    { 
        cv.Optional(CONF_SWITCH_DATAPOINT): cv.uint8_t, 
        
    }
)

CONFIG_SCHEMA = (
    ECONET_CLIENT_SCHEMA.extend(
        {
            cv.Optional(CONF_ENABLE_SWITCH): ECONET_SWITCH_SCHEMA,
            cv.Optional(CONF_CC_DHUM_ENABLE_STATE): ECONET_SWITCH_SCHEMA,
        }
    )
    .extend(cv.COMPONENT_SCHEMA).extend(cv.polling_component_schema("5s"))
)
#async def to_code(config):
  #  """Generate main.cpp code"""
  #  var = await switch.new_switch(config)
    # var = cg.new_Pvariable(config[CONF_ID])
 #   await cg.register_component(var, config)
 #   econet_var = await cg.get_variable(config[CONF_ECONET_ID])
 #   cg.add(var.set_econet(econet_var))
  #  cg.add(var.set_switch_id(config[CONF_SWITCH_DATAPOINT]))

async def to_code(config):
    var = await cg.get_variable(config[CONF_ECONET_ID])
    for key in SWITCHES:
        if key in config:
            conf = config[key]
            #var = cg.new_Pvariable(conf[CONF_ID])
            #var = await cg.get_variable(conf[CONF_ID])
            sens = await switch.new_switch(conf)
            await cg.register_component(var, conf)
            cg.add(sens.set_econet(var))
           # cg.add(getattr(hub, f"set_{key}_number")(var))
            #cg.add(var.set_switch_id(config[CONF_SWITCH_DATAPOINT])) if key == CONF_ENABLE_SWITCH else ''
            #cg.add(sens.set_econet(var))
            #cg.add(getattr(econet_var, f"set_{key}_switch")(var))
#async def to_code(config): 
    #var = await cg.get_variable(config[CONF_ECONET_ID])
    
    #if CONF_ENABLE_SWITCH in config:
      #conf = config[CONF_ENABLE_SWITCH]
      #sens = await switch.new_switch(conf)
      #await cg.register_component(sens, conf)
      #cg.add(sens.set_econet(var))
      
    #if CONF_CC_DHUM_ENABLE_STATE in config:
      #conf = config[CONF_CC_DHUM_ENABLE_STATE]
      #sens = await switch.new_switch(conf)
      #await cg.register_component(sens, conf)
      #cg.add(sens.set_econet(var))