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

  void set_cc_blower_cfm_sensor(sensor::Sensor *sensor) { this->cc_blower_cfm_sensor_ = sensor; }
  void set_cc_blower_rpm_sensor(sensor::Sensor *sensor) { this->cc_blower_rpm_sensor_ = sensor; }

 protected:
  std::map<std::string, sensor::Sensor *> float_sensors_;
  std::map<std::string, sensor::Sensor *> enum_sensors_;

  sensor::Sensor *cc_blower_cfm_sensor_{nullptr};
  sensor::Sensor *cc_blower_rpm_sensor_{nullptr};
};

}  // namespace econet
}  // namespace esphome
