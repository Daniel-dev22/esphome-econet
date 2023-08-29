#pragma once

#include "esphome/components/text_sensor/text_sensor.h"
#include "../econet.h"

namespace esphome {
namespace econet {

class EconetTextSensor : public PollingComponent, public EconetClient {
 public:
  void update() override;
  void dump_config() override;

  void set_text_sensor(std::string key, text_sensor::TextSensor *sensor) { text_sensors_[key] = sensor; }

 protected:
  std::map<std::string, text_sensor::TextSensor *> text_sensors_;
};

}  // namespace econet
}  // namespace esphome
