#pragma once

#include "esphome/components/switch/switch.h"
#include "../econet.h"

namespace esphome {
namespace econet {

class EconetSwitch : public switch_::Switch, public PollingComponent {
 public:
  void update() override;
  void dump_config() override;
  void write_state(bool state) override;

  void set_econet(Econet *econet) { this->econet = econet; }

 protected:
  Econet *econet;
};

}  // namespace econet
}  // namespace esphome
