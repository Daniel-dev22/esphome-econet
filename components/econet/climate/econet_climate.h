#pragma once

#include <map>
#include "esphome/components/climate/climate.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"
#include "../econet.h"

namespace esphome {
namespace econet {

class EconetClimate : public climate::Climate, public PollingComponent, public EconetClient {
 public:
  void update() override;
  void dump_config() override;
  void control(const climate::ClimateCall &call) override;

 protected:
  climate::ClimateTraits traits() override;
};

}  // namespace econet
}  // namespace esphome
