#include "econet_binary_sensor.h"

namespace esphome {
namespace econet {

static const char *const TAG = "econet.bsensor";

void EconetBinarySensor::update() {
  for (const auto &kv : this->binary_sensors_) {
    kv.second->publish_state(this->econet->get_int_value(kv.first) == 1);
  }
}

void EconetBinarySensor::dump_config() { ESP_LOGCONFIG(TAG, "Econet Sensors:"); }

}  // namespace econet
}  // namespace esphome
