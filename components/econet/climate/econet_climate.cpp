#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/components/climate/climate_traits.h"
#include "econet_climate.h"

using namespace esphome;

namespace esphome {
namespace econet {

namespace {

float fahrenheit_to_celsius(float f) { return (f - 32) * 5 / 9; }
float celsius_to_fahrenheit(float c) { return c * 9 / 5 + 32; }

}  // namespace

#define HVAC_SETPOINT_MIN 10
#define HVAC_SETPOINT_MAX 32
#define WATER_HEATER_SETPOINT_MIN 43.3333
#define WATER_HEATER_SETPOINT_MAX 60
#define SETPOINT_STEP 1.0f

static const char *const TAG = "econet.climate";

void EconetClimate::dump_config() {
  ESP_LOGCONFIG(TAG, "EconetClimate:");
  ESP_LOGCONFIG(TAG, "  Update interval: %u", this->get_update_interval());
  this->dump_traits_(TAG);
}

// Supported Model Types: enum ModelType { MODEL_TYPE_TANKLESS = 0, MODEL_TYPE_HEATPUMP, MODEL_TYPE_HVAC };
climate::ClimateTraits EconetClimate::traits() {
  auto traits = climate::ClimateTraits();

  // Set Universal Traits
  traits.set_supports_action(false);
  traits.set_supports_current_temperature(true);
  traits.set_visual_temperature_step(SETPOINT_STEP);

  // Conifugre Climate Traits for the Different Device Types (MODEL_TYPE_TANKLESS, MODEL_TYPE_HEATPUMP, MODEL_TYPE_HVAC)
  if (this->econet->get_model_type() == MODEL_TYPE_HVAC) { // HVAC Systems
    traits.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_COOL, climate::CLIMATE_MODE_HEAT,
                                climate::CLIMATE_MODE_HEAT_COOL, climate::CLIMATE_MODE_FAN_ONLY});
    traits.set_supports_two_point_target_temperature(true);
    traits.set_visual_min_temperature(HVAC_SETPOINT_MIN);
    traits.set_visual_max_temperature(HVAC_SETPOINT_MAX);
    traits.set_supported_custom_fan_modes({"Automatic", "Speed 1 (Low)", "Speed 2 (Medium Low)", "Speed 3 (Medium)",
                                           "Speed 4 (Medium High)", "Speed 5 (High)"});
  } else { // Hot Water Heaters
    traits.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_AUTO});
    traits.set_supports_two_point_target_temperature(false);
    traits.set_visual_min_temperature(WATER_HEATER_SETPOINT_MIN);
    traits.set_visual_max_temperature(WATER_HEATER_SETPOINT_MAX);
  }

  // Set Custom Presets Specific to Hybrid Heat Pumps
  if (this->econet->get_model_type() == MODEL_TYPE_HEATPUMP) {
    traits.set_supported_custom_presets({"Off", "Eco Mode", "Heat Pump", "High Demand", "Electric", "Vacation"});
  }

  return traits;
}

void EconetClimate::update() {
  ModelType model_type = this->econet->get_model_type();
  if (model_type == MODEL_TYPE_HVAC) {
    this->target_temperature_low = fahrenheit_to_celsius(this->econet->get_float_value("HEATSETP"));
    this->target_temperature_high = fahrenheit_to_celsius(this->econet->get_float_value("COOLSETP"));
    this->current_temperature = fahrenheit_to_celsius(this->econet->get_float_value("SPT_STAT"));
  } else {
    this->target_temperature = fahrenheit_to_celsius(this->econet->get_float_value("WHTRSETP"));
    this->current_temperature = fahrenheit_to_celsius(
        this->econet->get_float_value(model_type == MODEL_TYPE_HEATPUMP ? "UPHTRTMP" : "TEMP_OUT"));
  }
  if (model_type == MODEL_TYPE_HVAC) {
    switch (this->econet->get_int_value("STATMODE")) {
      case 0:
        this->mode = climate::CLIMATE_MODE_HEAT;
        break;
      case 1:
        this->mode = climate::CLIMATE_MODE_COOL;
        break;
      case 2:
        this->mode = climate::CLIMATE_MODE_HEAT_COOL;
        break;
      case 3:
        this->mode = climate::CLIMATE_MODE_FAN_ONLY;
        break;
      case 4:
        this->mode = climate::CLIMATE_MODE_OFF;
        break;
    }
    switch (this->econet->get_int_value("STATNFAN")) {
      case 0:
        this->set_custom_fan_mode_("Automatic");
        break;
      case 1:
        this->set_custom_fan_mode_("Speed 1 (Low)");
        break;
      case 2:
        this->set_custom_fan_mode_("Speed 2 (Medium Low)");
        break;
      case 3:
        this->set_custom_fan_mode_("Speed 3 (Medium)");
        break;
      case 4:
        this->set_custom_fan_mode_("Speed 4 (Medium High)");
        break;
      case 5:
        this->set_custom_fan_mode_("Speed 5 (High)");
        break;
    }
  } else {
    if (this->econet->get_int_value("WHTRENAB") == 1) {
      this->mode = climate::CLIMATE_MODE_AUTO;
    } else {
      this->mode = climate::CLIMATE_MODE_OFF;
    }
  }
  if (model_type == MODEL_TYPE_HEATPUMP) {
    switch (this->econet->get_int_value("WHTRCNFG")) {
      case 0:
        this->set_custom_preset_("Off");
        break;
      case 1:
        this->set_custom_preset_("Eco Mode");
        break;
      case 2:
        this->set_custom_preset_("Heat Pump");
        break;
      case 3:
        this->set_custom_preset_("High Demand");
        break;
      case 4:
        this->set_custom_preset_("Electric");
        break;
      case 5:
        this->set_custom_preset_("Vacation");
        break;
      default:
        this->set_custom_preset_("Off");
    }
  }
  this->publish_state();
}

void EconetClimate::control(const climate::ClimateCall &call) {
  if (call.get_target_temperature_low().has_value()) {
    this->econet->write_float_value("HEATSETP", celsius_to_fahrenheit(call.get_target_temperature_low().value()));
  }

  if (call.get_target_temperature_high().has_value()) {
    this->econet->write_float_value("COOLSETP", celsius_to_fahrenheit(call.get_target_temperature_high().value()));
  }

  if (call.get_target_temperature().has_value()) {
    this->econet->write_float_value("WHTRSETP", celsius_to_fahrenheit(call.get_target_temperature().value()));
  }

  if (call.get_mode().has_value()) {
    climate::ClimateMode climate_mode = call.get_mode().value();
    if (this->econet->get_model_type() == MODEL_TYPE_HVAC) {
      uint8_t new_mode = 0;

      switch (climate_mode) {
        case climate::CLIMATE_MODE_HEAT_COOL:
          new_mode = 2;
          break;
        case climate::CLIMATE_MODE_HEAT:
          new_mode = 0;
          break;
        case climate::CLIMATE_MODE_COOL:
          new_mode = 1;
          break;
        case climate::CLIMATE_MODE_FAN_ONLY:
          new_mode = 3;
          break;
        case climate::CLIMATE_MODE_OFF:
          new_mode = 4;
          break;
        default:
          new_mode = 4;
      }
      ESP_LOGI("econet", "Raw Mode is %d", climate_mode);
      ESP_LOGI("econet", "Lets change the mode to %d", new_mode);
      this->econet->write_int_value("STATMODE", new_mode);
    } else {
      bool new_mode = climate_mode != climate::CLIMATE_MODE_OFF;
      ESP_LOGI("econet", "Raw Mode is %d", climate_mode);
      ESP_LOGI("econet", "Lets change the mode to %d", new_mode);
      this->econet->write_int_value("WHTRENAB", new_mode);
    }
  }

  if (call.get_custom_fan_mode().has_value()) {
    std::string fan_mode = call.get_custom_fan_mode().value();
    int new_fan_mode = 0;
    if (fan_mode == "Automatic") {
      new_fan_mode = 0;
    } else if (fan_mode == "Speed 1 (Low)") {
      new_fan_mode = 1;
    } else if (fan_mode == "Speed 2 (Medium Low)") {
      new_fan_mode = 2;
    } else if (fan_mode == "Speed 3 (Medium)") {
      new_fan_mode = 3;
    } else if (fan_mode == "Speed 4 (Medium High)") {
      new_fan_mode = 4;
    } else if (fan_mode == "Speed 5 (High)") {
      new_fan_mode = 5;
    }
    this->econet->write_int_value("STATNFAN", new_fan_mode);
  }

  if (call.get_custom_preset().has_value()) {
    std::string preset = call.get_custom_preset().value();

    ESP_LOGI("econet", "Set custom preset: %s", preset);

    uint8_t new_mode = -1;

    if (preset == "Off") {
      new_mode = 0;
    } else if (preset == "Eco Mode") {
      new_mode = 1;
    } else if (preset == "Heat Pump") {
      new_mode = 2;
    } else if (preset == "High Demand") {
      new_mode = 3;
    } else if (preset == "Electric") {
      new_mode = 4;
    } else if (preset == "Vacation") {
      new_mode = 5;
    }

    if (new_mode != -1) {
      this->econet->write_int_value("WHTRCNFG", new_mode);
    }
  }
}

}  // namespace econet
}  // namespace esphome
