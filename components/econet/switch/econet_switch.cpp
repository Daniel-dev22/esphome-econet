#include "econet_switch.h"

namespace esphome {
namespace econet {

static const char *const TAG = "econet.switch";
	
void EconetSwitch::update() {
	if(this->econet->get_type_id() == 1)
	{
		this->publish_state(this->econet->get_enable_state());
	}
	else if(this->econet->get_type_id() == 2)
	{
		this->publish_state(this->econet->get_cc_dhum_enable_state());
	}
}
void EconetSwitch::write_state(bool state) {
	ESP_LOGD("econet", "write_state");
	if(this->econet != nullptr)
	{
		if(this->econet->get_type_id() == 1)
		{
			ESP_LOGD("econet", "econet is good");
			this->econet->set_enable_state(state);
		}
		else if(this->econet->get_type_id() == 2)
		{
			this->econet->set_dhum_enable_state(state);
		}
	}
}

void EconetSwitch::dump_config() {

}

}  // namespace econet
}  // namespace esphome
