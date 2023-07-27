#pragma once
 
#include "esphome/core/component.h"
#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"
#include "esphome/components/number/number.h"
#ifndef "../econet.h"
#define "../econet.h"

namespace esphome {
namespace econet {

class EconetNumber : public number::Number, PollingComponent, public EconetClient {
 public:
	void update() override;
	void dump_config() override;
	
	void set_cc_dhumsetp_number(number::Number *number) {
		this->cc_dhumsetp_number_ = number;
	}
	
 protected:
  // number::NumberTraits traits() override;
  number::Number *cc_dhumsetp_number_{nullptr};
};

}  // namespace daikin_s21
}  // namespace esphome
#endif