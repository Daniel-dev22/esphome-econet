#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"
#include "econet_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace econet {

static const char *const TAG = "econet.number";

void EconetNumber::dump_config() { LOG_NUMBER("", "EconetNumber:", this); }

void EconetNumber::update() {
  if (this->econet->is_ready()) {
    this->publish_state(this->econet->get_cc_dhumsetp());
    ESP_LOGD("econet", "number update sent: %f", this->econet->get_cc_dhumsetp());
  }
}

void EconetNumber::control(float new_dhumsetp) {
  if (this->econet != nullptr) {
    this->econet->set_new_dhumsetp(new_dhumsetp);
    this->publish_state(new_dhumsetp);
    ESP_LOGD("econet", "number command sent: %f", new_dhumsetp);
  }
}

}  // namespace econet
}  // namespace esphome
