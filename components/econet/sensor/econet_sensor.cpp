#include "econet_sensor.h"

namespace esphome {
namespace econet {

static const char *const TAG = "econet.sensor";

void EconetSensor::update() {
  for (const auto &kv : this->float_sensors_) {
    float value = this->econet->get_float_value(kv.first);
    if (kv.first == "FLOWRATE") {
      // Convert liters per minute to gpm
      value /= 3.785;
    }
    kv.second->publish_state(value);
  }

  for (const auto &kv : this->enum_sensors_) {
    kv.second->publish_state(this->econet->get_int_value(kv.first));
  }
}

void EconetSensor::dump_config() { ESP_LOGCONFIG(TAG, "Econet Sensors:"); }

}  // namespace econet
}  // namespace esphome
