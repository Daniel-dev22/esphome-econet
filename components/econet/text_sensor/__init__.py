from esphome.components import text_sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_SENSOR_DATAPOINT
from .. import econet_ns, CONF_ECONET_ID, Econet

DEPENDENCIES = ["econet"]

EconetTextSensor = econet_ns.class_(
    "EconetTextSensor", text_sensor.TextSensor, cg.Component
)

CONFIG_SCHEMA = (
    text_sensor.text_sensor_schema()
    .extend(
        {
            cv.GenerateID(): cv.declare_id(EconetTextSensor),
            cv.GenerateID(CONF_ECONET_ID): cv.use_id(Econet),
            cv.Required(CONF_SENSOR_DATAPOINT): cv.string,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await text_sensor.new_text_sensor(config)
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_ECONET_ID])
    cg.add(var.set_econet_parent(paren))

    cg.add(var.set_sensor_id(config[CONF_SENSOR_DATAPOINT]))
