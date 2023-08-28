#pragma once

#include "esphome/core/component.h"
#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"
#include "esphome/components/number/number.h"
#include "../econet.h"

namespace esphome {
namespace econet {

class EconetNumber : public number::Number, public PollingComponent, public EconetClient {
 public:
  void update() override;
  void dump_config() override;
  void control(float new_dhumsetp) override;
};

}  // namespace econet
}  // namespace esphome
