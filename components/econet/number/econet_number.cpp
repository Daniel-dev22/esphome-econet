#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"
#include "econet_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace econet {

static const char *const TAG = "econet.number";

void EconetNumber::dump_config() { LOG_NUMBER("", "EconetNumber:", this); }

void EconetNumber::update() { this->publish_state(this->econet->get_float_value("DHUMSETP")); }

void EconetNumber::control(float new_dhumsetp) {
  if (this->econet == nullptr) {
    return;
  }
  if (this->econet->get_model_type() == MODEL_TYPE_HVAC) {
    this->econet->write_float_value("DHUMSETP", new_dhumsetp);
  }
  this->publish_state(new_dhumsetp);
}

}  // namespace econet
}  // namespace esphome
