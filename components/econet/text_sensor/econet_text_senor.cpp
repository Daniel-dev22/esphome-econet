#include "econet_text_sensor.h"

namespace esphome {
namespace econet {

static const char *const TAG = "econet.text_sensor";

void EconetTextSensor::update() {
  for (const auto &kv : this->text_sensors_) {
    kv.second->publish_state(this->econet->get_string_value(kv.first));
  }
}

void EconetTextSensor::dump_config() { ESP_LOGCONFIG(TAG, "Econet Text Sensors:"); }

}  // namespace econet
}  // namespace esphome
