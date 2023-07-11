#pragma once

#include "esphome/components/text_sensor/text_sensor.h"
#include "../econet.h"

namespace esphome {
namespace econet {

class EconetSensor : public PollingComponent, public EconetClient {
 public:
	void update() override;
	void dump_config() override;


	void set_cc_hvacmode_text_text_sensor(text_sensor::TextSensor *text_sensor) {
		this->cc_hvacmode_text_text_sensor_ = text_sensor;
	}

	
 protected:
	text_sensor::TextSensor *cc_hvacmode_text_text_sensor_{nullptr};
};

}  // namespace econet
}  // namespace esphome