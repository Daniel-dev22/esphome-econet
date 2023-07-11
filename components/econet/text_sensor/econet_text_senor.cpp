#include "econet_text_sensor.h"

namespace esphome {
namespace econet {

static const char *const TAG = "econet.text_sensor";

void EconetTextSensor::update() {
	if (!this->econet->is_ready())
		return;
	
	// ESP_LOGD("econet", "econet->get_type_id() = %d", this->econet->get_type_id());
	
	if (this->cc_hvacmode_text_text_sensor_ != nullptr) {
		this->cc_hvacmode_text_text_sensor_->publish_state(this->econet->get_cc_hvacmode_text());
	}
}

void EconetTextSensor::dump_config() {
	ESP_LOGCONFIG(TAG, "Econet Text Sensors:");
	// LOG_SENSOR("  ", "temp_in", this->temp_in_sensor_);
	// LOG_SENSOR("  ", "temp_out", this->temp_out_sensor_);
}

}  // namespace econet
}  // namespace esphome
