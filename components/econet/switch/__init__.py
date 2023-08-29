"""
Switch component for Econet
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_SWITCH_DATAPOINT

from .. import CONF_ECONET_ID, ECONET_CLIENT_SCHEMA, EconetClient, econet_ns

DEPENDENCIES = ["econet"]

EconetSwitch = econet_ns.class_(
    "EconetSwitch", switch.Switch, cg.PollingComponent, EconetClient
)

CONF_CC_DHUM_ENABLE_STATE = "cc_dhum_enable_state"

SWITCHES = [CONF_CC_DHUM_ENABLE_STATE]


ECONET_SWITCH_SCHEMA = switch.switch_schema(EconetSwitch).extend(
    {
        cv.Optional(CONF_SWITCH_DATAPOINT): cv.uint8_t,
    }
)

CONFIG_SCHEMA = (
    ECONET_CLIENT_SCHEMA.extend(
        {
            cv.Optional(CONF_CC_DHUM_ENABLE_STATE): ECONET_SWITCH_SCHEMA,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(cv.polling_component_schema("5s"))
)


async def to_code(config):
    var = await cg.get_variable(config[CONF_ECONET_ID])

    if CONF_CC_DHUM_ENABLE_STATE in config:
        conf = config[CONF_CC_DHUM_ENABLE_STATE]
        sens = await switch.new_switch(conf)
        await cg.register_component(sens, conf)
        cg.add(sens.set_econet(var))
