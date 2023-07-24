#pragma once
 
#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"
#include "econet_number.h"
#include "esphome/core/log.h"
#include "esphome/components/number/number_traits.h"
#include "esphome/components/number/number_call.h"
 
namespace esphome {
namespace econet {
 
static const char *const TAG = "econet.number";

void EconetNumber::dump_config() {
  ESP_LOGCONFIG(TAG, "EconetNumber:");
  ESP_LOGCONFIG(TAG, "  Update interval: %u", this->get_update_interval());
  this->dump_traits_(TAG);
}

number::NumberTraits EconetNumber::traits() {
	auto traits = number::NumberTraits();
	
	// traits.set_min_value(20);
	// traits.set_max_value(70);
	// traits.set_step(1);
	traits.set_mode("slider");
	
return traits;
}

void EconetNumber::update() {
	if (this->econet->is_ready())
		{
			this->publish_state(this->econet->get_cc_dhumsetp());
		}
}

void EconetNumber::write_state(float new_dhumsetp) {
	if(this->econet != nullptr)
	{
		this->econet->set_new_dhumsetp(new_dhumsetp);
	}
}
 
}  // namespace econet
}  // namespace esphome
 
// https://esphome.io/api/number__call_8cpp_source.html