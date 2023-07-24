#pragma once
 
#include "esphome/core/component.h"
#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"
#include "esphome/components/number/number_call.h"
#include "esphome/components/number/number_traits.h"
 
namespace econet {

class EconetNumber : public number_::Number, public PollingComponent {
 public:
	void update() override;
	void dump_config() override;
	void write_state(float state) override;
	
  
 protected:
  number::NumberTraits traits() override;
};

}  // namespace daikin_s21
}  // namespace esphome
