from esphome.components import binary_sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_SENSOR_DATAPOINT

from .. import econet_ns, CONF_ECONET_ID, Econet

DEPENDENCIES = ["econet"]

EconetBinarySensor = econet_ns.class_(
    "EconetBinarySensor", binary_sensor.BinarySensor, cg.Component
)

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema(EconetBinarySensor)
    .extend(
        {
            cv.GenerateID(CONF_ECONET_ID): cv.use_id(Econet),
            cv.Required(CONF_SENSOR_DATAPOINT): cv.string,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_ECONET_ID])
    cg.add(var.set_econet_parent(paren))

    cg.add(var.set_sensor_id(config[CONF_SENSOR_DATAPOINT]))
