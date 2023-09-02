import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

CONF_UART = "uart"
CONF_MODEL = "model"
CONF_HVAC_WIFI_MODULE_CONNECTED = "hvac_wifi_module_connected"

econet_ns = cg.esphome_ns.namespace("econet")
Econet = econet_ns.class_("Econet", cg.Component, uart.UARTDevice)
EconetClient = econet_ns.class_("EconetClient")
uart_ns = cg.esphome_ns.namespace("uart")
UARTComponent = uart_ns.class_("UARTComponent")
ModelType = econet_ns.enum("ModelType")
MODEL_TYPES = {
    "Tankless": ModelType.MODEL_TYPE_TANKLESS,
    "Heatpump": ModelType.MODEL_TYPE_HEATPUMP,
    "HVAC": ModelType.MODEL_TYPE_HVAC,
}

CONF_ECONET_ID = "econet_id"
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Econet),
            cv.Required(CONF_MODEL): cv.enum(MODEL_TYPES),
            cv.Optional(CONF_HVAC_WIFI_MODULE_CONNECTED, default=True): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)

ECONET_CLIENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ECONET_ID): cv.use_id(Econet),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    cg.add(var.set_model_type(config[CONF_MODEL]))
    cg.add(var.set_hvac_wifi_module_connected(config[CONF_HVAC_WIFI_MODULE_CONNECTED]))
