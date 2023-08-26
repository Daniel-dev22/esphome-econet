"""
Sensor component for Econet
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_MOISTURE,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_WATER,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_WATER,
    STATE_CLASS_MEASUREMENT,

)

from .. import (
    econet_ns,
    CONF_ECONET_ID,
    ECONET_CLIENT_SCHEMA,
    EconetClient,
)

EconetSensor = econet_ns.class_(
    "EconetSensor", cg.PollingComponent, EconetClient
)


CONF_ECONET_ID = "econet"
CONF_TEMP_IN = "temp_in"
CONF_TEMP_OUT = "temp_out"
CONF_SETPOINT = "setpoint"
CONF_FLOW_RATE = "flow_rate"
CONF_WATER_USED = "water_used"
CONF_BTUS_USED = "btus_used"
CONF_IGNITION_CYCLES = "ignition_cycles"
CONF_INSTANT_BTUS = "instant_btus"
CONF_HOT_WATER = "hot_water"
CONF_AMBIENTT = "ambient_temp"
CONF_LOHTRTMP = "lower_water_heater_temp"
CONF_UPHTRTMP = "upper_water_heater_temp"
CONF_POWRWATT = "power_watt"
CONF_EVAPTEMP = "evap_temp"
CONF_SUCTIONT = "suction_temp"
CONF_DISCTEMP = "discharge_temp"

CONF_CC_HVACMODE = "cc_hvacmode"
CONF_CC_SPT_STAT = "cc_spt_stat"
CONF_CC_COOLSETP = "cc_coolsetp"
CONF_CC_AUTOMODE = "cc_automode"
CONF_CC_REL_HUM = "cc_rel_hum"
CONF_CC_BLOWER_CFM = "cc_blower_cfm"
CONF_CC_BLOWER_RPM = "cc_blower_rpm"

CONF_HVAC_ODU_OUTSIDE_AIR_TEMP = "hvac_odu_outside_air_temp"
CONF_HVAC_ODU_EVAPORATOR_TEMP = "hvac_odu_evaporator_temp"
CONF_HVAC_ODU_INVERTER_CRANK_SPEED = "hvac_odu_inverter_crank_speed"
CONF_HVAC_ODU_CRANKCASE_HEATER_TEMP = "hvac_odu_crankcase_heater_temp"
CONF_HVAC_ODU_EXV_CURRENT_POSITION = "hvac_odu_exv_current_position"
CONF_HVAC_ODU_EXV_SUPER_HEAT = "hvac_odu_exv_super_heat"
CONF_HVAC_ODU_SUCTION_LINE_TEMP = "hvac_odu_suction_line_temp"
CONF_HVAC_ODU_PRESSURE_SUCTION = "hvac_odu_pressure_suction"

SENSORS = [
    CONF_TEMP_IN,
    CONF_TEMP_OUT,
    CONF_SETPOINT,
    CONF_FLOW_RATE,
    CONF_WATER_USED,
    CONF_BTUS_USED,
    CONF_IGNITION_CYCLES,
    CONF_INSTANT_BTUS,
    CONF_HOT_WATER,
    CONF_AMBIENTT,
    CONF_LOHTRTMP,
    CONF_UPHTRTMP,
    CONF_POWRWATT,
    CONF_EVAPTEMP,
    CONF_SUCTIONT,
    CONF_DISCTEMP,
    CONF_HVAC_ODU_OUTSIDE_AIR_TEMP,
    CONF_HVAC_ODU_EVAPORATOR_TEMP,
    CONF_HVAC_ODU_INVERTER_CRANK_SPEED,
    CONF_HVAC_ODU_CRANKCASE_HEATER_TEMP,
    CONF_HVAC_ODU_EXV_CURRENT_POSITION,
    CONF_HVAC_ODU_EXV_SUPER_HEAT,
    CONF_HVAC_ODU_SUCTION_LINE_TEMP,
    CONF_HVAC_ODU_PRESSURE_SUCTION,
    CONF_CC_HVACMODE,
    CONF_CC_SPT_STAT,
    CONF_CC_COOLSETP,
    CONF_CC_AUTOMODE,
    CONF_CC_REL_HUM,
    CONF_CC_BLOWER_CFM,
    CONF_CC_BLOWER_RPM
]


CONFIG_SCHEMA = (
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_TEMP_IN): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_TEMP_OUT): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_SETPOINT): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_FLOW_RATE): sensor.sensor_schema(
                unit_of_measurement="gpm",
                accuracy_decimals=1,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                icon=ICON_WATER,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_WATER_USED): sensor.sensor_schema(
                unit_of_measurement="gal",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_WATER,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_BTUS_USED): sensor.sensor_schema(
                unit_of_measurement="kbtu",
                accuracy_decimals=3,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_IGNITION_CYCLES): sensor.sensor_schema(
                unit_of_measurement="",
                accuracy_decimals=0,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_INSTANT_BTUS): sensor.sensor_schema(
                unit_of_measurement="kbtu/hr",
                accuracy_decimals=3,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_HOT_WATER): sensor.sensor_schema(
                unit_of_measurement="%",
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_MOISTURE,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_AMBIENTT): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_LOHTRTMP): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_UPHTRTMP): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_POWRWATT): sensor.sensor_schema(
                unit_of_measurement="W",
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_EVAPTEMP): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_SUCTIONT): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_DISCTEMP): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_CC_SPT_STAT): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_CC_COOLSETP): sensor.sensor_schema(
                unit_of_measurement="°F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_CC_HVACMODE): sensor.sensor_schema(
                unit_of_measurement="",
                accuracy_decimals=0,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_CC_AUTOMODE): sensor.sensor_schema(
                unit_of_measurement="",
                accuracy_decimals=0,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_CC_REL_HUM): sensor.sensor_schema(
                unit_of_measurement="%",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_CC_BLOWER_CFM): sensor.sensor_schema(
                unit_of_measurement="",
                accuracy_decimals=0,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
		{
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_CC_BLOWER_RPM): sensor.sensor_schema(
                unit_of_measurement="",
                accuracy_decimals=0,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_HVAC_ODU_OUTSIDE_AIR_TEMP): sensor.sensor_schema(
                unit_of_measurement="F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_HVAC_ODU_EVAPORATOR_TEMP): sensor.sensor_schema(
                unit_of_measurement="F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_HVAC_ODU_INVERTER_CRANK_SPEED): sensor.sensor_schema(
                unit_of_measurement="",
                accuracy_decimals=1,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_HVAC_ODU_CRANKCASE_HEATER_TEMP): sensor.sensor_schema(
                unit_of_measurement="F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_HVAC_ODU_EXV_CURRENT_POSITION): sensor.sensor_schema(
                unit_of_measurement="",
                accuracy_decimals=1,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_HVAC_ODU_EXV_CURRENT_POSITION): sensor.sensor_schema(
                unit_of_measurement="",
                accuracy_decimals=1,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_HVAC_ODU_EXV_SUPER_HEAT): sensor.sensor_schema(
                unit_of_measurement="F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_HVAC_ODU_SUCTION_LINE_TEMP): sensor.sensor_schema(
                unit_of_measurement="F",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        },
        {
            cv.GenerateID(): cv.declare_id(EconetSensor),
            cv.Optional(CONF_HVAC_ODU_PRESSURE_SUCTION): sensor.sensor_schema(
                unit_of_measurement="PSI",
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_PRESSURE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            )
        }
    )
    .extend(ECONET_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("5s"))
)




async def to_code(config):
    """Generate main.cpp code"""

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    econet_var = await cg.get_variable(config[CONF_ECONET_ID])
    cg.add(var.set_econet(econet_var))




    for key in SENSORS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(var,f"set_{key}_sensor")(sens))