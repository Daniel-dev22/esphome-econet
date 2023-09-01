#include "econet.h"

using namespace esphome;

namespace esphome {
namespace econet {

static const char *const TAG = "econet";

uint16_t gen_crc16(const uint8_t *data, uint16_t size) {
  uint16_t out = 0;
  int bits_read = 0, bit_flag;

  /* Sanity check: */
  if (data == NULL)
    return 0;

  while (size > 0) {
    bit_flag = out >> 15;

    /* Get next bit: */
    out <<= 1;
    out |= (*data >> bits_read) & 1;  // item a) work from the least significant bits

    /* Increment bit counter: */
    bits_read++;
    if (bits_read > 7) {
      bits_read = 0;
      data++;
      size--;
    }

    /* Cycle check: */
    if (bit_flag)
      out ^= 0x8005;
  }

  // item b) "push out" the last 16 bits
  int i;
  for (i = 0; i < 16; ++i) {
    bit_flag = out >> 15;
    out <<= 1;
    if (bit_flag)
      out ^= 0x8005;
  }

  // item c) reverse the bits
  uint16_t crc = 0;
  i = 0x8000;
  int j = 0x0001;
  for (; i != 0; i >>= 1, j <<= 1) {
    if (i & out)
      crc |= j;
  }

  return crc;
}

float bytesToFloat(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
  uint8_t byte_array[] = {b3, b2, b1, b0};
  float result;
  std::copy(reinterpret_cast<const char *>(&byte_array[0]), reinterpret_cast<const char *>(&byte_array[4]),
            reinterpret_cast<char *>(&result));
  return result;
}

uint32_t floatToUint32(float f) {
  uint32_t fbits = 0;
  memcpy(&fbits, &f, sizeof fbits);

  return fbits;
}

#define ECONET_BAUD_RATE 38400

void Econet::check_uart_settings() {
  for (auto uart : {this->econet_uart}) {
    if (uart->get_baud_rate() != ECONET_BAUD_RATE) {
      ESP_LOGE(TAG,
               "  Invalid baud_rate: Integration requested baud_rate %u but you "
               "have %u!",
               ECONET_BAUD_RATE, uart->get_baud_rate());
    }
  }
}

void Econet::dump_config() { this->check_uart_settings(); }

void Econet::handle_binary(uint32_t src_adr, std::string obj_string, std::vector<uint8_t> data) {
  if (src_adr == FURNACE) {
    if (obj_string == "HWSTATUS") {
      uint8_t heatcmd = data[11];  // 0 to 100 [0, 70, 100]
      uint8_t coolcmd = data[12];  // [0, 2] Maybe 1 for 1st Stage?
      uint16_t fan_cfm = (((uint16_t) data[13]) * 256) + data[14];
      ESP_LOGI("econet", "  HeatCmd : %d %", heatcmd);
      ESP_LOGI("econet", "  CoolCmd : %d %", coolcmd);
      ESP_LOGI("econet", "  FanCFM? : %d cfm", fan_cfm);
    }
  } else if (src_adr == AIR_HANDLER) {
    if (obj_string == "AIRHSTAT") {
      cc_blower_cfm = (data[16] << 8) + data[17];
      cc_blower_rpm = (data[20] << 8) + data[21];
    }
  }
}

void Econet::make_request() {
  uint32_t dst_adr = SMARTEC_TRANSLATOR;
  if (model_type_ == MODEL_TYPE_HEATPUMP) {
    dst_adr = HEAT_PUMP_WATER_HEATER;
  } else if (model_type_ == MODEL_TYPE_HVAC) {
    dst_adr = CONTROL_CENTER;
  }

  uint8_t dst_bus = 0x00;

  uint32_t src_adr = WIFI_MODULE;

  uint8_t src_bus = 0x00;

  std::vector<uint8_t> data;

  if (!this->pending_float_writes_.empty()) {
    const auto &kv = this->pending_float_writes_.begin();
    this->write_value(dst_adr, src_adr, kv->first, FLOAT, kv->second);
    this->pending_float_writes_.erase(kv->first);
    return;
  }
  if (!this->pending_int_writes_.empty()) {
    const auto &kv = this->pending_int_writes_.begin();
    this->write_value(dst_adr, src_adr, kv->first, ENUM_TEXT, kv->second);
    this->pending_int_writes_.erase(kv->first);
    return;
  }

  std::vector<std::string> str_ids{};

  if (req_id == 0) {
    if (model_type_ == MODEL_TYPE_TANKLESS) {
      str_ids.push_back("FLOWRATE");
      str_ids.push_back("TEMP_OUT");
      str_ids.push_back("TEMP__IN");
      str_ids.push_back("WHTRENAB");
      str_ids.push_back("WHTRMODE");
      str_ids.push_back("WHTRSETP");
      str_ids.push_back("WTR_USED");
      str_ids.push_back("WTR_BTUS");
      str_ids.push_back("IGNCYCLS");
      str_ids.push_back("BURNTIME");
    } else if (model_type_ == MODEL_TYPE_HEATPUMP) {
      str_ids.push_back("WHTRENAB");
      str_ids.push_back("WHTRCNFG");
      str_ids.push_back("WHTRSETP");
      str_ids.push_back("HOTWATER");
      str_ids.push_back("HEATCTRL");
      str_ids.push_back("FAN_CTRL");
      str_ids.push_back("COMP_RLY");
      str_ids.push_back("AMBIENTT");
      str_ids.push_back("LOHTRTMP");
      str_ids.push_back("UPHTRTMP");
      str_ids.push_back("POWRWATT");
      str_ids.push_back("EVAPTEMP");
      str_ids.push_back("SUCTIONT");
      str_ids.push_back("DISCTEMP");
      str_ids.push_back("ALRMALRT");
      //str_ids.push_back("SUCTIONT");
      //str_ids.push_back("DISCTEMP");
    } else if (model_type_ == MODEL_TYPE_HVAC && hvac_wifi_module_connected_ == false) {
      str_ids.push_back("DHUMSETP");
      str_ids.push_back("DHUMENAB");
      str_ids.push_back("DH_DRAIN");
      str_ids.push_back("OAT_TEMP");
      str_ids.push_back("COOLSETP");
      str_ids.push_back("HEATSETP");
      str_ids.push_back("STATNFAN");
      str_ids.push_back("STATMODE");
      str_ids.push_back("AUTOMODE");
      str_ids.push_back("HVACMODE");
      str_ids.push_back("RELH7005");
      str_ids.push_back("SPT_STAT");
    }
    if (model_type_ != MODEL_TYPE_HVAC || !hvac_wifi_module_connected_) {
      request_strings(dst_adr, src_adr, str_ids);
    }
    return;
  }

  if (req_id == 1) {
    if (model_type_ == MODEL_TYPE_TANKLESS) {
      str_ids.push_back("HWSTATUS");
      request_strings(FURNACE, CONTROL_CENTER, str_ids);
    }
    return;
  }
}

void Econet::parse_tx_message() { this->parse_message(true); }

void Econet::parse_rx_message() { this->parse_message(false); }

void Econet::parse_message(bool is_tx) {
  bool logvals = true;

  uint8_t pdata[255];

  // Receive?
  uint32_t dst_adr = 0;
  uint8_t dst_bus = 0;
  uint32_t src_adr = 0;
  uint8_t src_bus = 0;
  uint8_t data_len = 0;
  uint8_t command = 0;
  uint16_t crc = 0;
  uint16_t crc_check = 0;

  uint16_t pmsg_len = 0;

  const uint8_t *b = is_tx ? wbuffer : buffer;
  dst_adr = ((b[0] & 0x7f) << 24) + (b[1] << 16) + (b[2] << 8) + b[3];
  dst_bus = b[4];

  src_adr = ((b[5] & 0x7f) << 24) + (b[6] << 16) + (b[7] << 8) + b[8];
  src_bus = b[9];

  data_len = b[10];

  pmsg_len = data_len + MSG_HEADER_SIZE + MSG_CRC_SIZE;

  command = b[13];

  crc = (b[data_len + MSG_HEADER_SIZE]) + (b[data_len + MSG_HEADER_SIZE + 1] << 8);

  crc_check = gen_crc16(b, data_len + 14);

  for (int i = 0; i < data_len; i++) {
    pdata[i] = b[MSG_HEADER_SIZE + i];
  }

  ESP_LOGI("econet", "%s %s", is_tx ? ">>>" : "<<<", format_hex_pretty(b, pmsg_len).c_str());

  ESP_LOGI("econet", "  Dst Adr : 0x%x", dst_adr);
  ESP_LOGI("econet", "  Src Adr : 0x%x", src_adr);
  ESP_LOGI("econet", "  Length  : %d", data_len);
  ESP_LOGI("econet", "  Command : %d", command);
  ESP_LOGI("econet", "  Data    : %s", format_hex_pretty((const uint8_t *) pdata, data_len).c_str());

  // Track Read Requests
  if (command == READ_COMMAND) {
    uint8_t type = pdata[0];
    uint8_t prop_type = pdata[1];

    ESP_LOGI("econet", "  Type    : %d", type);
    ESP_LOGI("econet", "  PropType: %d", prop_type);

    std::vector<std::string> obj_names;

    if (type == 1) {
      if (prop_type == 1) {
        // pdata[2] = 0
        // pdata[3] = 0
        char char_arr[data_len - 4];

        for (int a = 0; a < data_len - 4; a++) {
          char_arr[a] = pdata[a + 4];
        }

        std::string s(char_arr, sizeof(char_arr));

        obj_names.push_back(s);

        ESP_LOGI("econet", "  %s", s.c_str());
      } else {
        ESP_LOGI("econet", "  Don't Currently Support This Property Type", prop_type);
      }
      read_req.dst_adr = dst_adr;
      read_req.src_adr = src_adr;
      read_req.obj_names = obj_names;
      read_req.awaiting_res = true;
    } else if (type == 2) {
      if (prop_type == 1) {
        int start = 4;
        int end = -1;
        int str_len = -1;
        for (int tpos = 5; tpos < data_len; tpos++) {
          bool mflag = false;
          if (pdata[tpos - 1] == 0 && pdata[tpos] == 0) {
            // This detects a 00 00
            str_len = tpos - start - 1;
            mflag = true;
          }
          if (tpos + 1 >= data_len) {
            str_len = tpos - start + 1;
            mflag = true;
          }

          if (mflag == true) {
            if (str_len > 0) {
              char char_arr[str_len];

              for (int a = 0; a < str_len; a++) {
                if (start + a > 0 && start + a < data_len) {
                  char_arr[a] = pdata[start + a];
                }
              }

              std::string s(char_arr, sizeof(char_arr));

              obj_names.push_back(s);

              ESP_LOGI("econet", "  %s", s.c_str());
            }
            start = tpos + 1;
          }
        }
      } else {
        ESP_LOGI("econet", "  Don't Currently Support This Property Type", prop_type);
      }

      read_req.dst_adr = dst_adr;
      read_req.src_adr = src_adr;
      read_req.obj_names = obj_names;
      read_req.awaiting_res = true;
    } else {
      ESP_LOGI("econet", "  Don't Currently Support This Class Type", type);
    }

  } else if (command == ACK) {
    if (read_req.dst_adr == src_adr && read_req.src_adr == dst_adr && read_req.awaiting_res == true) {
      if (read_req.obj_names.size() == 1) {
        uint8_t item_type = pdata[0] & 0x7F;
        if (item_type == 4) {
          std::vector<uint8_t> dest;

          for (int i = 0; i < data_len; i++) {
            dest.push_back(pdata[i]);
          }
          handle_binary(src_adr, read_req.obj_names[0], dest);
        }
      } else {
        int tpos = 0;
        uint8_t item_num = 0;

        while (tpos < data_len) {
          uint8_t item_len = pdata[tpos];
          uint8_t item_type = pdata[tpos + 1] & 0x7F;

          if (item_type == 0 && tpos + 7 < data_len) {
            float item_value = bytesToFloat(pdata[tpos + 4], pdata[tpos + 5], pdata[tpos + 6], pdata[tpos + 7]);

            if (item_num < read_req.obj_names.size()) {
              // TODO: detect change and push update
              this->values_float_[read_req.obj_names[item_num]] = item_value;
              ESP_LOGI("econet", "  %s : %f", read_req.obj_names[item_num].c_str(), item_value);
            }
          } else if (item_type == 1) {
            // Decode text only
            uint8_t item_text_len = item_len - 4;

            if (item_text_len > 0) {
              char char_arr[item_text_len];

              for (int a = 0; a < item_text_len; a++) {
                if (tpos + a + 4 < data_len) {
                  char_arr[a] = pdata[tpos + a + 4];
                }
              }

              std::string s(char_arr, sizeof(char_arr));

              if (item_num < read_req.obj_names.size()) {
                // handle_text(src_adr, read_req.obj_names[item_num], s);
                ESP_LOGI("econet", "  %s : (%s)", read_req.obj_names[item_num].c_str(), s.c_str());
              }
            }
          } else if (item_type == 2 && tpos + 5 < data_len) {
            // Enumerated Text

            uint8_t item_value = pdata[tpos + 4];

            uint8_t item_text_len = pdata[tpos + 5];

            if (item_text_len > 0) {
              char char_arr[item_text_len];

              for (int a = 0; a < item_text_len; a++) {
                if (tpos + a + 6 < data_len) {
                  char_arr[a] = pdata[tpos + a + 6];
                }
              }

              std::string s(char_arr, sizeof(char_arr));
              if (item_num < read_req.obj_names.size()) {
                // TODO: detect change and push update
                this->values_int_[read_req.obj_names[item_num]] = item_value;
                this->values_string_[read_req.obj_names[item_num]] = s;
                ESP_LOGI("econet", "  %s : %d (%s)", read_req.obj_names[item_num].c_str(), item_value, s.c_str());
              }
            }
          }
          tpos += item_len + 1;
          item_num++;
        }
      }
      // This is likely the response to our request and now we "know" what was requested!
      // ESP_LOGI("econet", "  RESPONSE RECEIVED!!!");
      // for(int a = 0; a < read_req.obj_names.size(); a++)
      // {
      // ESP_LOGI("econet", "  ValName : %s", read_req.obj_names[a].c_str());
      // }
      read_req.awaiting_res = false;
    }
  } else if (command == WRITE_COMMAND) {
    uint8_t type = pdata[0];
    uint8_t prop_type = pdata[1];

    ESP_LOGI("econet", "  ClssType: %d", type);
    ESP_LOGI("econet", "  PropType: %d", prop_type);

    if (type == 1) {
      // WRITE DATA
      uint8_t datatype = pdata[2];

      if (datatype == 0 || datatype == 2) {
        // FLOAT
        if (datatype == 0) {
          ESP_LOGI("econet", "  DataType: %d (Float)", datatype);
        } else {
          ESP_LOGI("econet", "  DataType: %d (Enum Text)", datatype);
        }

        if (data_len == 18) {
          int strlen = 8;
          char char_arr[strlen];
          int start = 6;

          for (int a = 0; a < strlen; a++) {
            if (start + a > 0 && start + a < data_len) {
              char_arr[a] = pdata[start + a];
            }
          }

          std::string s(char_arr, sizeof(char_arr));

          int tpos = start + strlen;

          float item_value = bytesToFloat(pdata[tpos + 0], pdata[tpos + 1], pdata[tpos + 2], pdata[tpos + 3]);

          ESP_LOGI("econet", "  %s: %f", s.c_str(), item_value);
        } else {
          ESP_LOGI("econet", "  Unexpected Write Data Length", datatype);
        }

        // 01.01.00.07.00.00.4F.43.4F.4D.4D.41.4E.44.42.48.00.00
        // Object - OCOMMAND
        //
      } else if (datatype == 2) {
        // ENUM TEXT

      } else if (datatype == 4) {
        // BYTES
        ESP_LOGI("econet", "  DataType: %d (Bytes)", datatype);

        // if(
      } else {
        ESP_LOGI("econet", "  DataType: %d", datatype);
      }
    } else if (type == 7) {
      // TIME AND DATA
    }

    else if (type == 9) {
      // SYSTEM HANDLER - LISTING OF ADDRESSES ON BUS
      // 09.01.00.00.03.80.00.00.03.40.00.00.05.00
      // 00 00 03 80
      // 00 00 03 40
      // 00 00 05 00
    } else {
    }
  }
}

void Econet::read_buffer(int bytes_available) {
  if (bytes_available > 1200) {
    ESP_LOGI("econet", "BA=%d,LT=%d ms", bytes_available, this->act_loop_time_);
  }

  // Limit to Read 1200 bytes
  int bytes_to_read = std::min(bytes_available, 1200);

  uint8_t bytes[bytes_to_read];

  if (!econet_uart->read_array(bytes, bytes_to_read)) {
    return;
  }

  for (int i = 0; i < bytes_to_read; i++) {
    uint8_t byte = bytes[i];
    buffer[pos] = byte;
    if ((pos == DST_ADR_POS || pos == SRC_ADR_POS) && byte != 0x80) {
      pos = 0;
      continue;
    }
    if (pos == LEN_POS) {
      data_len = byte;
      msg_len = data_len + MSG_HEADER_SIZE + MSG_CRC_SIZE;
    }
    pos++;

    if (pos == msg_len && msg_len != 0) {
      // We have a full message
      this->parse_rx_message();
      pos = 0;
      msg_len = 0;
      data_len = 0;
    }
  }
}

void Econet::loop() {
  const uint32_t now = millis();

  // Wait at least 10ms since last attempt to read
  if (now - this->last_read_ <= 10) {
    return;
  }

  this->act_loop_time_ = now - this->last_read_;
  this->last_read_ = now;

  // Read Everything that is in the buffer
  int bytes_available = econet_uart->available();
  if (bytes_available > 0) {
    this->last_read_data_ = now;
    ESP_LOGI("econet", "Read %d. ms=%d, lt=%d", bytes_available, now, act_loop_time_);
    this->read_buffer(bytes_available);
    return;
  }

  // Wait at least 100ms since last time we read data
  if (now - this->last_read_data_ <= 100) {
    return;
  }

  // Wait at least 500ms since last request
  if (now - this->last_request_ <= 500) {
    return;
  }

  // If there are any needed write requests they will be made every 500ms.
  // Read requests are made every 1s unless there are pending write requests in
  // which case they will be at the next available 500ms slot.

  // Bus is assumed Available For Sending
  ESP_LOGI("econet", "request ms=%d", now);
  this->last_request_ = now;
  this->make_request();
  req_id++;
  if (req_id > 1) {
    ESP_LOGI("econet", "request ms=%d", now);
    this->last_request_ = now;
    this->make_request();
    req_id = 0;
  }
}

void Econet::write_value(uint32_t dst_adr, uint32_t src_adr, std::string object, uint8_t type, float value) {
  std::vector<uint8_t> data;

  data.push_back(1);
  data.push_back(1);
  // Sometimes this is 2 and sometimes this is 0
  data.push_back(type);
  data.push_back(1);
  data.push_back(0);
  data.push_back(0);

  std::vector<uint8_t> sdata(object.begin(), object.end());

  for (int j = 0; j < 8; j++) {
    if (j < object.length()) {
      data.push_back(sdata[j]);
    } else {
      data.push_back(0);
    }
  }

  uint32_t f_to_32 = floatToUint32(value);

  data.push_back((uint8_t) (f_to_32 >> 24));
  data.push_back((uint8_t) (f_to_32 >> 16));
  data.push_back((uint8_t) (f_to_32 >> 8));
  data.push_back((uint8_t) (f_to_32));

  transmit_message(dst_adr, src_adr, WRITE_COMMAND, data);
}

void Econet::request_strings(uint32_t dst_adr, uint32_t src_adr, std::vector<std::string> objects) {
  std::vector<uint8_t> data;

  int num_of_strs = objects.size();

  if (num_of_strs > 1) {
    // Read Class
    data.push_back(2);
  } else {
    data.push_back(1);
  }

  // Read Property
  data.push_back(1);

  int a = 0;

  for (int i = 0; i < num_of_strs; i++) {
    data.push_back(0);
    data.push_back(0);

    std::string my_str = objects[i];

    std::vector<uint8_t> sdata(my_str.begin(), my_str.end());
    uint8_t *p = &sdata[0];

    for (int j = 0; j < 8; j++) {
      if (j < objects[i].length()) {
        data.push_back(sdata[j]);
      } else {
        data.push_back(0);
      }
    }
  }
  transmit_message(dst_adr, src_adr, READ_COMMAND, data);
}

void Econet::transmit_message(uint32_t dst_adr, uint32_t src_adr, uint8_t command, std::vector<uint8_t> data) {
  uint8_t dst_bus = 0;
  uint8_t src_bus = 0;
  uint16_t wdata_len = data.size();

  wbuffer[0] = 0x80;
  wbuffer[1] = (uint8_t) (dst_adr >> 16);
  wbuffer[2] = (uint8_t) (dst_adr >> 8);
  wbuffer[3] = (uint8_t) dst_adr;
  wbuffer[4] = dst_bus;

  wbuffer[5] = 0x80;
  wbuffer[6] = (uint8_t) (src_adr >> 16);
  wbuffer[7] = (uint8_t) (src_adr >> 8);
  wbuffer[8] = (uint8_t) src_adr;
  wbuffer[9] = src_bus;

  wbuffer[10] = wdata_len;
  wbuffer[11] = 0;
  wbuffer[12] = 0;
  wbuffer[13] = command;

  for (int i = 0; i < wdata_len; i++) {
    wbuffer[14 + i] = data[i];
  }

  uint16_t crc = gen_crc16(wbuffer, wdata_len + 14);

  wbuffer[wdata_len + 14] = (uint8_t) crc;
  wbuffer[wdata_len + 14 + 1] = (uint8_t) (crc >> 8);

  econet_uart->write_array(wbuffer, wdata_len + 14 + 2);
  // econet_uart->flush();

  parse_tx_message();
}

void Econet::send_datapoint(uint8_t datapoint_id, float value) {
  for (auto &listener : this->listeners_) {
    if (listener.datapoint_id == datapoint_id)
      listener.on_datapoint(value);
  }
}

void Econet::dump_state() {}

void Econet::register_listener(uint8_t datapoint_id, const std::function<void(float)> &func) {
  auto listener = DatapointListener{
      .datapoint_id = datapoint_id,
      .on_datapoint = func,
  };
  this->listeners_.push_back(listener);
}

}  // namespace econet
}  // namespace esphome
