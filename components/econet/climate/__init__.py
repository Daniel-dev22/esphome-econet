from esphome.components import climate
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID

from .. import econet_ns, CONF_ECONET_ID, Econet

DEPENDENCIES = ["econet"]

EconetClimate = econet_ns.class_("EconetClimate", climate.Climate, cg.Component)

CONFIG_SCHEMA = cv.All(
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(EconetClimate),
            cv.GenerateID(CONF_ECONET_ID): cv.use_id(Econet),
        }
    ).extend(cv.COMPONENT_SCHEMA),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    paren = await cg.get_variable(config[CONF_ECONET_ID])
    cg.add(var.set_econet_parent(paren))
