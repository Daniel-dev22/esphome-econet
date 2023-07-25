#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"
#include "econet_number.h"
#include "esphome/core/log.h"
#include "esphome/components/number/number_traits.h"
#include "esphome/components/number/number_call.h"
#include "esphome/components/number/number.h"
 
namespace esphome {
namespace econet {
 
static const char *const TAG = "econet.number";

void EconetNumber::dump_config() {
	LOG_NUMBER("", "EconetNumber:", this);
}

 /* number::NumberTraits EconetNumber::traits() {
	auto traits = number::NumberTraits();
	
	traits.set_min_value(min_value);
	traits.set_max_value(max_value);
	traits.set_step(step);
	traits.set_mode("slider");
	
 return traits;
*/// }

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