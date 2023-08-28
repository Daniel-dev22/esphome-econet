#pragma once

#include "esphome/components/sensor/sensor.h"
#include "../econet.h"
#include <map>

namespace esphome {
namespace econet {

class EconetSensor : public PollingComponent, public EconetClient {
 public:
  void update() override;
  void dump_config() override;

  void set_float_sensor(std::string key, sensor::Sensor *sensor) { float_sensors_[key] = sensor; }
  void set_enum_sensor(std::string key, sensor::Sensor *sensor) { enum_sensors_[key] = sensor; }

 protected:
  std::map<std::string, sensor::Sensor *> float_sensors_;
  std::map<std::string, sensor::Sensor *> enum_sensors_;
};

}  // namespace econet
}  // namespace esphome
