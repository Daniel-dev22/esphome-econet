#include "econet_switch.h"

namespace esphome {
namespace econet {

static const char *const TAG = "econet.switch";

void EconetSwitch::update() {
  if (this->econet->get_model_type() == MODEL_TYPE_HVAC) {
    this->publish_state(this->econet->get_int_value("DHUMENAB") == 1);
  }
}
void EconetSwitch::write_state(bool state) {
  ESP_LOGD("econet", "write_state");
  if (this->econet == nullptr) {
    return;
  }
  if (this->econet->get_model_type() == MODEL_TYPE_HVAC) {
    this->econet->write_int_value("DHUMENAB", state);
  }
}

void EconetSwitch::dump_config() {}

}  // namespace econet
}  // namespace esphome
