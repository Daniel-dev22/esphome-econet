#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "../econet.h"

namespace esphome {
namespace econet {

class EconetBinarySensor : public PollingComponent, public EconetClient {
 public:
  void update() override;
  void dump_config() override;

  void set_binary_sensor(std::string key, binary_sensor::BinarySensor *sensor) { binary_sensors_[key] = sensor; }

 protected:
  std::map<std::string, binary_sensor::BinarySensor *> binary_sensors_;
};

}  // namespace econet
}  // namespace esphome
