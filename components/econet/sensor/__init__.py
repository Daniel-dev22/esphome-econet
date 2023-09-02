from esphome.components import sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID, CONF_SENSOR_DATAPOINT
from .. import econet_ns, CONF_ECONET_ID, Econet

DEPENDENCIES = ["econet"]

EconetSensor = econet_ns.class_("EconetSensor", sensor.Sensor, cg.Component)

CONFIG_SCHEMA = (
    sensor.sensor_schema(EconetSensor)
    .extend(
        {
            cv.GenerateID(CONF_ECONET_ID): cv.use_id(Econet),
            cv.Required(CONF_SENSOR_DATAPOINT): cv.string,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)

    paren = await cg.get_variable(config[CONF_ECONET_ID])
    cg.add(var.set_econet_parent(paren))

    cg.add(var.set_sensor_id(config[CONF_SENSOR_DATAPOINT]))
