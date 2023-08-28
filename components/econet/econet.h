#pragma once

#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include <map>
#ifndef MAINECONETH
#define MAINECONETH

namespace esphome {
namespace econet {

enum ModelType { MODEL_TYPE_TANKLESS = 0, MODEL_TYPE_HEATPUMP, MODEL_TYPE_HVAC };

class ReadRequest {
 public:
  uint32_t dst_adr;
  uint8_t dst_bus;

  uint32_t src_adr;
  uint8_t src_bus;

  bool awaiting_res = false;
  // uint8_t read_class;
  // uint8_t read_prop;

  std::vector<std::string> obj_names;
};

struct DatapointListener {
  uint8_t datapoint_id;
  std::function<void(float)> on_datapoint;
};

class Econet : public Component {
 public:
  void loop() override;
  void dump_config() override;

  void set_model_type(ModelType model_type) { model_type_ = model_type; }
  ModelType get_model_type() { return model_type_; }

  void set_uart(uart::UARTComponent *econet_uart) { this->econet_uart = econet_uart; }
  void set_hvac_wifi_module_connected(bool hvac_wifi_module_connected) {
    hvac_wifi_module_connected_ = hvac_wifi_module_connected;
  }

  float get_float_value(std::string key) { return values_float_[key]; }
  uint8_t get_int_value(std::string key) { return values_int_[key]; }
  std::string get_string_value(std::string key) { return values_string_[key]; }

  void write_float_value(std::string key, float value) { pending_float_writes_[key] = value; }
  void write_int_value(std::string key, uint8_t value) { pending_int_writes_[key] = value; }

  void register_listener(uint8_t datapoint_id, const std::function<void(float)> &func);

 protected:
  ModelType model_type_;
  std::vector<DatapointListener> listeners_;
  ReadRequest read_req;
  void dump_state();
  void check_uart_settings();
  void send_datapoint(uint8_t datapoint_id, float value);

  void make_request();
  void read_buffer(int bytes_available);
  void parse_message(bool is_tx);
  void parse_rx_message();
  void parse_tx_message();

  void handle_binary(uint32_t src_adr, std::string obj_string, std::vector<uint8_t> data);

  void transmit_message(uint32_t dst_adr, uint32_t src_adr, uint8_t command, std::vector<uint8_t> data);
  void request_strings(uint32_t dst_adr, uint32_t src_adr, std::vector<std::string> objects);
  void write_value(uint32_t dst_adr, uint32_t src_adr, std::string object, uint8_t type, float value);

  uart::UARTComponent *econet_uart{nullptr};

  std::map<std::string, float> values_float_{};
  std::map<std::string, uint8_t> values_int_{};
  std::map<std::string, std::string> values_string_{};

  std::map<std::string, float> pending_float_writes_{};
  std::map<std::string, uint8_t> pending_int_writes_{};

  bool hvac_wifi_module_connected_ = true;

  uint8_t req_id = 0;
  uint32_t last_request_{0};
  uint32_t last_read_{0};
  uint32_t last_read_data_{0};
  uint32_t act_loop_time_{0};
  uint8_t data_len = 0;
  uint16_t msg_len = 0;
  int pos = 0;
  static const int max_message_size = 271;
  uint8_t buffer[max_message_size];

  uint8_t wbuffer[max_message_size];
  uint16_t wmsg_len = 0;

  uint32_t COMPUTER = 192;                   // 80 00 00 C0
  uint32_t FURNACE = 0x1c0;                  // 80 00 01 C0
  uint32_t UNKNOWN_HANDLER = 241;            // 80 00 00 F1
  uint32_t WIFI_MODULE = 832;                // 80 00 03 40
  uint32_t SMARTEC_TRANSLATOR = 4160;        // 80 00 10 40
  uint32_t INTERNAL = 4736;                  // 80 00 10 40
  uint32_t HEAT_PUMP_WATER_HEATER = 0x1280;  // 80 00 12 80
  uint32_t AIR_HANDLER = 0x3c0;              // 80 00 03 C0
  uint32_t CONTROL_CENTER = 0x380;           // 80 00 03 80
  uint32_t OUTDOOR_UNIT = 0x400;

  uint8_t DST_ADR_POS = 0;
  uint8_t SRC_ADR_POS = 5;
  uint8_t SRC_BUS_POS = 9;
  uint8_t LEN_POS = 10;
  uint8_t COMMAND_POS = 13;
  uint8_t DATA_START_POS = 14;

  uint8_t MSG_HEADER_SIZE = 14;
  uint8_t BYTES_BETWEEN_ADRS = 5;
  uint8_t MSG_CRC_SIZE = 2;

  uint8_t ACK = 6;
  uint8_t READ_COMMAND = 30;
  uint8_t WRITE_COMMAND = 31;

  uint8_t FLOAT = 0;
  uint8_t ENUM_TEXT = 2;
};

class EconetClient {
 public:
  void set_econet(Econet *econet) { this->econet = econet; }

 protected:
  Econet *econet;
};

}  // namespace econet
}  // namespace esphome
#endif
