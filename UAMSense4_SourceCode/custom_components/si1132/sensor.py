
# SEE https://github.com/thegroove/esphome-custom-component-examples

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, ICON_EMPTY, UNIT_EMPTY

DEPENDENCIES = ['i2c']

CONF_I2C_ADDR = 0x60

CONF_LUMINOSITY = "luminosity"
CONF_INFRARED = "infrared"

si1132_ns = cg.esphome_ns.namespace('si1132')
SI1132Component = si1132_ns.class_('SI1132Component', cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SI1132Component),
            cv.Optional(CONF_LUMINOSITY): sensor.sensor_schema(unit_of_measurement=UNIT_EMPTY, icon=ICON_EMPTY, accuracy_decimals=5),
            cv.Optional(CONF_INFRARED): sensor.sensor_schema(unit_of_measurement=UNIT_EMPTY, icon=ICON_EMPTY, accuracy_decimals=5),
        }
    )
    .extend(cv.polling_component_schema("10s"))
    .extend(i2c.i2c_device_schema(CONF_I2C_ADDR))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    
    if CONF_LUMINOSITY in config:
        conf = config[CONF_LUMINOSITY]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_luminosity_sensor(sens))

    if CONF_INFRARED in config:
        conf = config[CONF_INFRARED]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_infrared_sensor(sens))
