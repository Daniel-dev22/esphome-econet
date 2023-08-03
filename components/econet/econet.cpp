#include "econet.h"

using namespace esphome;

namespace esphome {
namespace econet {

static const char *const TAG = "econet";

uint16_t gen_crc16(const uint8_t *data, uint16_t size)
{
	uint16_t out = 0;
	int bits_read = 0, bit_flag;

	/* Sanity check: */
	if(data == NULL)
		return 0;

	while(size > 0)
	{
		bit_flag = out >> 15;

		/* Get next bit: */
		out <<= 1;
		out |= (*data >> bits_read) & 1; // item a) work from the least significant bits

		/* Increment bit counter: */
		bits_read++;
		if(bits_read > 7)
		{
			bits_read = 0;
			data++;
			size--;
		}

		/* Cycle check: */
		if(bit_flag)
			out ^= 0x8005;

	}

	// item b) "push out" the last 16 bits
	int i;
	for (i = 0; i < 16; ++i) {
		bit_flag = out >> 15;
		out <<= 1;
		if(bit_flag)
			out ^= 0x8005;
	}

	// item c) reverse the bits
	uint16_t crc = 0;
	i = 0x8000;
	int j = 0x0001;
	for (; i != 0; i >>=1, j <<= 1) {
		if (i & out) crc |= j;
	}

	return crc;
}
float bytesToFloat(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) 
{ 
	uint8_t byte_array[] = { b3, b2, b1, b0 };
	float result;
	std::copy(reinterpret_cast<const char*>(&byte_array[0]),
			  reinterpret_cast<const char*>(&byte_array[4]),
			  reinterpret_cast<char*>(&result));
	return result;
} 
uint32_t floatToUint32(float f) 
{ 
	uint32_t fbits = 0;
	memcpy(&fbits, &f, sizeof fbits);
	
	return fbits;
} 
	
void Econet::set_uart(uart::UARTComponent *econet_uart) {
  this->econet_uart = econet_uart;
}

#define ECONET_BAUD_RATE 38400

void Econet::check_uart_settings() {
  for (auto uart : {this->econet_uart}) {
    if (uart->get_baud_rate() != ECONET_BAUD_RATE) {
      ESP_LOGE(
          TAG,
          "  Invalid baud_rate: Integration requested baud_rate %u but you "
          "have %u!",
          ECONET_BAUD_RATE, uart->get_baud_rate());
    }
  }
}

void Econet::dump_config() {
  this->check_uart_settings();
}	
void Econet::handle_float(uint32_t src_adr, std::string obj_string, float value)
{
	if(src_adr == SMARTEC_TRANSLATOR)
	{
		if(obj_string == "FLOWRATE")
		{
			flow_rate = value/3.785;
		}
		else if(obj_string == "TEMP_OUT")
		{
			temp_out = value;	
		}
		else if(obj_string == "TEMP__IN")
		{
			temp_in = value;	
		}
		else if(obj_string == "WHTRSETP")
		{
			setpoint = value;	
		}
		else if(obj_string == "WTR_USED")
		{
			water_used = value;	
		}
		else if(obj_string == "WTR_BTUS")
		{
			btus_used = value;	
		}
		else if(obj_string == "IGNCYCLS")
		{
			ignition_cycles = value;	
		}
		else if(obj_string == "BURNTIME")
		{
			// Not Supported Yet
		}
    else
    {
 			ESP_LOGI("econet", "  %s : not supported yet", obj_string.c_str());
    }
	}
	else if(src_adr == HEAT_PUMP_WATER_HEATER)
	{
		ESP_LOGI("econet", "  %s : not supported yet", obj_string.c_str());

	}
	else if(src_adr == CONTROL_CENTER)
	{
		if(obj_string == "SPT_STAT")
		{
			cc_spt_stat = value;
		}
		else if(obj_string == "COOLSETP")
		{
			cc_cool_setpoint = value;
			// setpoint
		}
		else if(obj_string == "COOLSETP")
		{
			cc_cool_setpoint = value;
		}
		else if(obj_string == "HEATSETP")
		{
			cc_heat_setpoint = value;
		}
		else if(obj_string == "RELH7005")
		{
			cc_rel_hum = value;
		}
		else if(obj_string == "DHUMSETP")
		{
			cc_dhumsetp = value;
			ESP_LOGI("econet", "  DHUMSETPCONFRIMED : %f ", cc_dhumsetp);
		}
    else if (obj_string == "OAT_TEMP")
       cc_outdoor_air_temp = value;
    else if (obj_string == "DH_DRAIN")
       cc_dehumidifier_drain = value;

    else{
   		ESP_LOGI("econet", "  %s : not supported yet", obj_string.c_str());

    }
	}
  else
  	ESP_LOGI("econet", "  %s : not supported yet", obj_string.c_str());

}
	/*
	
							if(item_num == 4)
						{
							if(item_value == 0)
							{
								enable_state = false;
								// send_datapoint(0,0);
							}
							else if(item_value == 1)
							{
								enable_state = true;
								// send_datapoint(0,1);
							}
						}
						else if(item_num == 5)
						{
							ESP_LOGD("econet", "WHTRMODE (val): %d", item_value);
							ESP_LOGD("econet", "WHTRMODE (str): %s", s.c_str());
						}
						
	*/
void Econet::handle_enumerated_text(uint32_t src_adr, std::string obj_string, uint8_t value, std::string text)
{
	if(src_adr == SMARTEC_TRANSLATOR)
	{
		if(obj_string == "WHTRENAB")
		{
			enable_state = value == 1;
		}
		else if(obj_string == "WHTRMODE")
		{
			// Not Supported
		}
	}
	else if(src_adr == HEAT_PUMP_WATER_HEATER)
	{
		
	}
	else if(src_adr == CONTROL_CENTER)
	{
		if(obj_string == "HVACMODE")
		{
			cc_hvacmode = value;
			cc_hvacmode_text = text.c_str();   //(example: 3 (High Cool))
		//	ESP_LOGI("econet", "  TextHVACMode : %s ", text.c_str());
		}
		else if(obj_string == "AUTOMODE")
		{
			cc_automode = value;
			cc_automode_text = text.c_str();
		}
		else if(obj_string == "STATMODE")
		{
			cc_statmode = value;
		}
		else if(obj_string == "STATNFAN")
		{
			cc_fan_mode = value;
		}
    else if (obj_string == "DHUMENAB")
			cc_dehumidifier_enabled = value == 1;
      
else
    		ESP_LOGI("econet", "  %s : not supported yet", obj_string.c_str());

	}
  else
  {
    		ESP_LOGI("econet", "  %s : not supported yet", obj_string.c_str());

  }
}
void Econet::handle_text(uint32_t src_adr, std::string obj_string, std::string text)
{
  		ESP_LOGI("econet", "  %s : not supported yet", obj_string.c_str());

	if(src_adr == 0x1040)
	{
	
	}
	else if(src_adr == CONTROL_CENTER)	
	{
	
	}

}

uint16_t convert_vector_to_uint16(uint16_t pos, std::vector<uint8_t> data)
{
  return (((uint16_t)data[pos]) * 256) + data[pos+1];
}

void Econet::handle_binary(uint32_t src_adr, std::string obj_string, std::vector<uint8_t> data)
{
	if(src_adr == FURNACE)
	{
		if(obj_string == "HWSTATUS")
		{
			// data[0] = 4
			// data[1 - 10] = 0
			uint8_t heatcmd = data[11]; // 0 to 100 [0, 70, 100]
			uint8_t coolcmd = data[12]; // [0, 2] Maybe 1 for 1st Stage?
			uint16_t fan_cfm = convert_vector_to_uint16(13,data);
			uint16_t fan_rpm = convert_vector_to_uint16(17,data);
      uint16_t max_cfm = convert_vector_to_uint16(41,data);

			ESP_LOGI("econet", "  HeatCmd : %d %", heatcmd);
			ESP_LOGI("econet", "  CoolCmd : %d %", coolcmd);
			
			ESP_LOGI("econet", "  FanCFM  : %d cfm", fan_cfm);
 			ESP_LOGI("econet", "  FanRPM  : %d rpm", fan_rpm);
 			ESP_LOGI("econet", "  MaxCFM  : %d cfm", max_cfm);

      ESP_LOGI("econet", "  MaxCFM  : %d cfm", max_cfm);


		}
    else
    {
      ESP_LOGI("econet", "  %s     : unsupported",obj_string.c_str());
    }

	}
	else if(src_adr == AIR_HANDLER)
	{
		if(obj_string == "AIRHSTAT")
		{
			// data[0] = 4
			// data[1 - 10] = 0
			cc_blower_cfm = (data[16] << 8) + data[17];
			cc_blower_rpm = (data[20] << 8) + data[21];
			uint16_t testone = (data[22] << 8) + data[23];
			uint16_t testtwo = (data[36] << 8) + data[37];
			uint16_t testthree = (data[57] << 8) + data[58];
			uint16_t testfour = (data[73] << 8) + data[74];
			uint16_t testfive = (data[95] << 8) + data[96];
			uint16_t testsix = (data[20] << 8) + data[21];
			uint16_t testseven = (data[22] << 8) + data[22];
			uint16_t testeight = (data[35] << 8) + data[36];
			uint16_t testnine = (data[56] << 8) + data[57];
			uint16_t testten = (data[72] << 8) + data[73];
			uint16_t testeleven = (data[94] << 8) + data[95];
			uint16_t testtwelve = (data[26] << 8) + data[27];
			uint16_t testthirteen = (data[102] << 8) + data[103];
			uint16_t testfourteen = (data[106] << 8) + data[107];
			uint16_t testfifteen = (data[140] << 8) + data[141];
			uint16_t testsixteen = (data[150] << 8) + data[151];
			// testtwo has something to do with the unit running when its off its 0
			
			ESP_LOGI("econet", "  TestOne : %d ", testone);
			ESP_LOGI("econet", "  TestTwo : %d ", testtwo);
			ESP_LOGI("econet", "  TestThree : %d ", testthree);
			ESP_LOGI("econet", "  TestFour : %d ", testfour);
			ESP_LOGI("econet", "  TestFive : %d ", testfive);
			ESP_LOGI("econet", "  TestSix : %d ", testsix);
			ESP_LOGI("econet", "  TestSeven : %d ", testseven);
			ESP_LOGI("econet", "  TestEight : %d ", testeight);
			ESP_LOGI("econet", "  TestNine : %d ", testnine);
			ESP_LOGI("econet", "  TestTen : %d ", testten);
			ESP_LOGI("econet", "  TestEleven : %d ", testeleven);
			ESP_LOGI("econet", "  TestTwelve : %d ", testtwelve);
			ESP_LOGI("econet", "  TestThirteen : %d ", testthirteen);
			ESP_LOGI("econet", "  TestFourteen : %d ", testfourteen);
			ESP_LOGI("econet", "  TestFifteen : %d ", testfifteen);
			ESP_LOGI("econet", "  TestSixteen : %d ", testsixteen);
			
			ESP_LOGI("econet", "  FanRPM? : %d rpm", cc_blower_rpm);
		}
    else
      ESP_LOGI("econet", "  %s     : unsupported",obj_string.c_str());
	}
  else if (src_adr==ZONE_CONTROL){
      if (obj_string == "ZONESTAT")
    {
      uint8_t zone1 = data[11] * 100.0/35; // multiply by 35 for calibration
			uint8_t zone2 = data[12] * 100.0/35; // 
      uint8_t zone3 = data[13] * 100.0/35; // 
    	ESP_LOGI("econet", "  Zone1Pct: %d%", zone1);
 			ESP_LOGI("econet", "  Zone2Pct: %d%", zone2);
 			ESP_LOGI("econet", "  Zone3Pct: %d%", zone3);
    }
        else
      ESP_LOGI("econet", "  %s     : unsupported",obj_string.c_str());

  }
  else
  {
      ESP_LOGI("econet", "  %s     : %s",obj_string.c_str(), format_hex_pretty((const uint8_t *) data.data(), data.size()).c_str());
  }
}
void Econet::make_request()
{	
	uint32_t dst_adr = SMARTEC_TRANSLATOR;
	if(type_id_ == 1) dst_adr = HEAT_PUMP_WATER_HEATER;
	else if(type_id_ == 2) dst_adr = CONTROL_CENTER;
	
	uint8_t dst_bus = 0x00;

	uint32_t src_adr = WIFI_MODULE;

	uint8_t src_bus = 0x00;

	std::vector<uint8_t> data;
	
	if(send_enable_disable == true)
	{
		this->write_value(dst_adr, src_adr, "WHTRENAB", ENUM_TEXT, static_cast<float>(enable_disable_cmd));
		
		send_enable_disable = false;
	}
	else if(send_new_mode == true)
	{
		// Modes
		// 0 - Off
		// 1 - Energy Saver
		// 2 - Heat Pump
		// 3 - High Demand
		// 4 - Electric / Gas
		if(this->type_id_ == 2)
		{
			this->write_value(dst_adr, src_adr, "STATMODE", ENUM_TEXT, new_mode);
		}
		else
		{
			this->write_value(dst_adr, src_adr, "WHTRCNFG", ENUM_TEXT, new_mode);
		}
		send_new_mode = false;
	}
	else if(send_new_setpoint_high == true)
	{
		if(this->type_id_ == 2)
		{
			this->write_value(dst_adr, src_adr, "COOLSETP", FLOAT, new_setpoint_high);
		}
		
		send_new_setpoint_high = false;
	}
	else if(send_new_setpoint_low == true)
	{
		if(this->type_id_ == 2)
		{
			this->write_value(dst_adr, src_adr, "HEATSETP", FLOAT, new_setpoint_low);
		}
		
		send_new_setpoint_low = false;
	}
	else if(send_new_setpoint == true)
	{
		if(this->type_id_ == 2)
		{
			this->write_value(dst_adr, src_adr, "COOLSETP", FLOAT, new_setpoint);
		}
		else
		{
			this->write_value(dst_adr, src_adr, "WHTRSETP", FLOAT, new_setpoint);
		}
		
		send_new_setpoint = false;
	}
	else if(send_new_fan_mode == true)
	{
		if(this->type_id_ == 2)
		{
			this->write_value(dst_adr, src_adr, "STATNFAN", ENUM_TEXT, new_fan_mode);
			cc_fan_mode = new_fan_mode;
		}
		
		send_new_fan_mode = false;
	}
	else if(send_new_dhumsetp == true)
	{
		if(this->type_id_ == 2)
		{
			this->write_value(dst_adr, src_adr, "DHUMSETP", FLOAT, new_dhumsetp);
		}
		
		send_new_dhumsetp = false;
	}
	else
	{
		std::vector<std::string> str_ids{};

		if(req_id == 0)
		{
			if(type_id_ == 0)
			{
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
			}
			else if(type_id_ == 1)
			{
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
			}
			else if(type_id_ == 2)
			{
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
				request_strings(dst_adr, src_adr, str_ids);
				/*
				str_ids.push_back("AAUX1CFM");
				str_ids.push_back("AAUX2CFM");
				str_ids.push_back("AAUX3CFM");
				str_ids.push_back("AAUX4CFM");
				*/
			}
			if(type_id_ != 2)
			{
				request_strings(dst_adr, src_adr, str_ids);
			}
		}
		else if(req_id == 1)
		{
			if(type_id_ == 0)
			{
				str_ids.push_back("HWSTATUS");
				request_strings(0x1c0, 0x380, str_ids);
			}
		}
	}
}
void Econet::parse_tx_message()
{
	this->parse_message(true);
}
void Econet::parse_rx_message()
{
	this->parse_message(false);
}
std::string Econet::get_readable_hardware_name(uint32_t device){
  if(device == COMPUTER) return "COMPUTER";
  else if (device == FURNACE) return "FURNACE";
  else if (device == WIFI_MODULE) return "WIFI_MODULE";
  else if (device == SMARTEC_TRANSLATOR ) return "SMARTEC_TRANSLATOR";
  else if (device == INTERNAL  		) return	         "INTERNAL";  			          
  else if (device == HEAT_PUMP_WATER_HEATER) return "HEAT_PUMP_WATER_HEATER";
  else if (device == AIR_HANDLER ) return			     "AIR_HANDLER";
  else if (device == CONTROL_CENTER) return 	   "CONTROL_CENTER"; 		      
  else if (device == ZONE_THERMOSTAT_2) return     "ZONE_THERMOSTAT_2";  
  else if (device == ZONE_THERMOSTAT_3) return    "ZONE_THERMOSTAT_3"  ;   
  else if (device == ZONE_CONTROL) return "ZONE_CONTROL"     ;
  else if (device == BROADCAST) return "BROADCAST"     ;
  else return "UNKNOWN";
  // else if (device == UNKNOWN) return               "UNKNOWN"  ;

  }
std::string Econet::get_readable_command_name(uint8_t command){
  if(command == ACK) return "ACK  ";
  else if (command == READ_COMMAND) return "READ ";
  else if (command == WRITE_COMMAND) return "WRITE";
  else return "UNK  ";
  }

void Econet::parse_message(bool is_tx)
{     
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
	
	if(is_tx == false)
	{
		dst_adr = ((buffer[0] & 0x7f) << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
		dst_bus = buffer[4];

		src_adr = ((buffer[5] & 0x7f) << 24) + (buffer[6] << 16) + (buffer[7] << 8) + buffer[8];
		src_bus = buffer[9];

		data_len = buffer[10];

		pmsg_len = data_len + MSG_HEADER_SIZE + MSG_CRC_SIZE;

		command = buffer[13];

		crc = (buffer[data_len + MSG_HEADER_SIZE]) + (buffer[data_len + MSG_HEADER_SIZE + 1] << 8);

		crc_check = gen_crc16(buffer, data_len + 14);

		for(int i = 0; i < data_len; i++)
		{
			pdata[i] = buffer[MSG_HEADER_SIZE + i];
		}
		
		// data_len + MSG_HEADER_SIZE + MSG_CRC_SIZE
		
	}
	else
	{
		dst_adr = ((wbuffer[0] & 0x7f) << 24) + (wbuffer[1] << 16) + (wbuffer[2] << 8) + wbuffer[3];
		dst_bus = wbuffer[4];

		src_adr = ((wbuffer[5] & 0x7f) << 24) + (wbuffer[6] << 16) + (wbuffer[7] << 8) + wbuffer[8];
		src_bus = wbuffer[9];

		data_len = wbuffer[10];

		pmsg_len = data_len + MSG_HEADER_SIZE + MSG_CRC_SIZE;

		command = wbuffer[13];

		crc = (wbuffer[data_len + MSG_HEADER_SIZE]) + (wbuffer[data_len + MSG_HEADER_SIZE + 1] << 8);

		crc_check = gen_crc16(wbuffer, data_len + 14);

		for(int i = 0; i < data_len; i++)
		{
			pdata[i] = wbuffer[MSG_HEADER_SIZE + i];
		}
	}
	
	if(is_tx == false)
	{
		ESP_LOGI("econet", "<<< %s", format_hex_pretty((const uint8_t *) buffer, pmsg_len).c_str());
	}
	else
	{
		ESP_LOGI("econet", ">>> %s", format_hex_pretty((const uint8_t *) wbuffer, pmsg_len).c_str());
	}
	
	ESP_LOGI("econet", "  Dst Adr : 0x%x", dst_adr);
	ESP_LOGI("econet", "  Src Adr : 0x%x", src_adr);
	ESP_LOGI("econet", "  Length  : %d", data_len);
	ESP_LOGI("econet", "  Command : %d %s", command, this->get_readable_command_name(command).c_str());
	ESP_LOGI("econet", "  Data    : %s", format_hex_pretty((const uint8_t *) pdata, data_len).c_str());
	


	// Track Read Requests
  // TODO write requests also need tracking.  seems like the econet_req isn't fully reset after a match is found.
	if(command == READ_COMMAND)
	{
		uint8_t class_type= pdata[0];
		uint8_t prop_type = pdata[1];



		ESP_LOGI("econet", "  ClssType: %d", class_type);
		ESP_LOGI("econet", "  PropType: %d", prop_type);
		
		std::vector<std::string> obj_names;
		
 		std::string data_string;


		if(class_type == 1)
		{
			if(prop_type == 1)
			{
				// pdata[2] = 0
				// pdata[3] = 0
				char char_arr[data_len - 4];

				for (int a = 0; a < data_len - 4; a++) {
					char_arr[a] = pdata[a+4];
				}

				std::string s(char_arr, sizeof(char_arr));
        econet_req.command_name = s.c_str();
				obj_names.push_back(s);

				ESP_LOGI("econet", "  %s", s.c_str());
        data_string = s;
			}
			else
			{
        econet_req.command_name = "UNSUPPORTED";
				ESP_LOGI("econet", "  Don't Currently Support This Property Type", prop_type);
        // data_string = "c1p"+std::to_string(prop_type) + 
			}
			econet_req.dst_adr = dst_adr;
			econet_req.src_adr = src_adr;
			econet_req.obj_names = obj_names;
			econet_req.awaiting_res = true;
		}
		else if(class_type == 2)
		{
			if(prop_type == 1)
			{
        econet_req.command_name = "READ";
				int start = 4;
				int end = -1;
				int str_len = -1;
				for(int tpos = 5; tpos < data_len; tpos++)
				{
					bool mflag = false;
					if(pdata[tpos-1] == 0 && pdata[tpos] == 0)
					{
						// This detects a 00 00 
						str_len = tpos - start - 1;
						mflag = true;
					}
					if(tpos + 1 >= data_len)
					{
						str_len = tpos - start + 1;
						mflag = true;
					}
					
					if(mflag == true)
					{
						if(str_len > 0)
						{
							char char_arr[str_len];

							for (int a = 0; a < str_len; a++) {
								if(start + a > 0 && start + a < data_len)
								{
									char_arr[a] = pdata[start+a];
								}
							}

							std::string s(char_arr, sizeof(char_arr));

							obj_names.push_back(s);

							ESP_LOGI("econet", "  %s", s.c_str());
						}
						start = tpos+1;
					}
				}
			}
			else
			{
				ESP_LOGI("econet", "  Don't Currently Support This Property Type", prop_type);
			}
			
			econet_req.dst_adr = dst_adr;
			econet_req.src_adr = src_adr;
			econet_req.obj_names = obj_names;
			econet_req.awaiting_res = true;
		}
		else
		{
			ESP_LOGI("econet", "  Don't Currently Support This Class Type", class_type);
		}

  	ESP_LOGI("econet", "  Read    : short: 0x%x:0x%x [%s:%s] : %s",src_adr,dst_adr,this->get_readable_hardware_name(src_adr).c_str(),this->get_readable_hardware_name(dst_adr).c_str(),format_hex_pretty((const uint8_t *) pdata, data_len).c_str());

		// tuple<uint32_t, uint32_t> req_tup(dst_adr,src_adr);
		
		// econet_reqs
		// This is a read request so let's track it so that when an ACK/response comes to this we know what data it is!
		// for(
		// econet_reqs_
	}
	else if(command == ACK)
	{
		if(econet_req.dst_adr == src_adr && econet_req.src_adr == dst_adr){
      if (econet_req.awaiting_res == true)
		  {
	    ESP_LOGI("econet", "  Ack     : short: 0x%x:0x%x [%s:%s] %s : %s",src_adr,dst_adr,this->get_readable_hardware_name(src_adr).c_str(),this->get_readable_hardware_name(dst_adr).c_str(),econet_req.command_name.c_str(), format_hex_pretty((const uint8_t *) pdata, data_len).c_str());

			if(econet_req.obj_names.size() == 1)
			{
				uint8_t item_type = pdata[0] & 0x7F;
				if(item_type == 4)
				{
					std::vector<uint8_t> dest;
						
					for(int i=0; i < data_len; i++)
					{
						dest.push_back(pdata[i]);
					}
					handle_binary(src_adr, econet_req.obj_names[0], dest);
				}
        ESP_LOGI("econet", "  Ack     : expected item_type 4, got %d",item_type);

			}
			else
			{
				int tpos = 0;
				uint8_t item_num = 0;

				while(tpos < data_len)
				{
					uint8_t item_len = pdata[tpos];
					uint8_t item_type = pdata[tpos+1] & 0x7F;

					if(item_type == 0 && tpos+7 < data_len)
					{
						float item_value = bytesToFloat(pdata[tpos+4],pdata[tpos+5],pdata[tpos+6],pdata[tpos+7]);
						
						if(item_num < econet_req.obj_names.size())
						{
							ESP_LOGI("econet", "  %s : %f", econet_req.obj_names[item_num].c_str(), item_value);
							handle_float(src_adr, econet_req.obj_names[item_num], item_value);
						}
					}
					else if(item_type == 1)
					{
						// Decode text only
						uint8_t item_text_len = item_len - 4;
						
						if(item_text_len > 0)
						{
							char char_arr[item_text_len];
							
							for (int a = 0; a < item_text_len; a++) {
								if(tpos+a+4 < data_len)
								{
									char_arr[a] = pdata[tpos+a+4];
								}
							}
							
							std::string s(char_arr, sizeof(char_arr));
							
							if(item_num < econet_req.obj_names.size())
							{
								handle_text(src_adr, econet_req.obj_names[item_num], s);
								ESP_LOGI("econet", "  %s : (%s)", econet_req.obj_names[item_num].c_str(), s.c_str());
							}
						}
					}
					else if(item_type == 2 && tpos+5 < data_len)
					{
						// Enumerated Text

						uint8_t item_value = pdata[tpos+4];

						uint8_t item_text_len = pdata[tpos+5];

						if(item_text_len > 0)
						{
							char char_arr[item_text_len];

							for (int a = 0; a < item_text_len; a++) {
								if(tpos+a+6 < data_len)
								{
									char_arr[a] = pdata[tpos+a+6];
								}
							}

							std::string s(char_arr, sizeof(char_arr));
							if(item_num < econet_req.obj_names.size())
							{
								handle_enumerated_text(src_adr, econet_req.obj_names[item_num], item_value, s);
								ESP_LOGI("econet", "  %s : %d (%s)", econet_req.obj_names[item_num].c_str(), item_value, s.c_str());
							}
						}
					}
					tpos += item_len+1;
					item_num++;
				}
			}
			// This is likely the response to our request and now we "know" what was requested!
			ESP_LOGI("econet", "  RESPONSE RECEIVED!!!");
			for(int a = 0; a < econet_req.obj_names.size(); a++)
			{
				ESP_LOGI("econet", "  ValName : %s", econet_req.obj_names[a].c_str());
			}
			econet_req.awaiting_res = false;			
		  }
      else {
	      ESP_LOGI("econet", "  AckMatch: short: 0x%x:0x%x [%s:%s] : %s",src_adr,dst_adr,this->get_readable_hardware_name(src_adr).c_str(),this->get_readable_hardware_name(dst_adr).c_str(),format_hex_pretty((const uint8_t *) pdata, data_len).c_str());
	    }
    }
    else
    	ESP_LOGI("econet", "  Ack Unk : short: 0x%x:0x%x [%s:%s] : %s",src_adr,dst_adr,this->get_readable_hardware_name(src_adr).c_str(),this->get_readable_hardware_name(dst_adr).c_str(),format_hex_pretty((const uint8_t *) pdata, data_len).c_str());
    
  }
	else if(command == WRITE_COMMAND)
	{

		econet_req.dst_adr = dst_adr;
		econet_req.src_adr = src_adr;
		std::vector<std::string> obj_names;
		econet_req.obj_names = obj_names;

		// econet_req.awaiting_res = true;

		uint8_t class_type= pdata[0];
		uint8_t prop_type = pdata[1];
    uint8_t pdata_withouttypes[255];

				for (int a = 0; a < data_len ; a++) {
					pdata_withouttypes[a] = pdata[a+2];
				}
    
  	// ESP_LOGI("econet", "  Write   : short: 0x%x:0x%x [%s:%s] [ct:%d,pt:%d]: %s",src_adr,dst_adr,
    //     class_type, prop_type,
    //     this->get_readable_hardware_name(src_adr).c_str(),
    //     this->get_readable_hardware_name(dst_adr).c_str(),
    //     format_hex_pretty((const uint8_t *) pdata_withouttypes, data_len-2).c_str());

  	ESP_LOGI("econet", "  Write   : short: 0x%x:0x%x [%s:%s] : %s",src_adr,dst_adr,this->get_readable_hardware_name(src_adr).c_str(),this->get_readable_hardware_name(dst_adr).c_str(),format_hex_pretty((const uint8_t *) pdata, data_len).c_str());

		ESP_LOGI("econet", "  ClssType: %d", class_type);
		ESP_LOGI("econet", "  PropType: %d", prop_type);
		
		if(class_type == 1)
		{
			// WRITE DATA
			uint8_t datatype = pdata[2];
			
			if(datatype == 0 || datatype == 2)
			{
				// FLOAT
				if(datatype == 0)
				{
					ESP_LOGI("econet", "  DataType: %d (Float)", datatype);
				}
				else
				{
					ESP_LOGI("econet", "  DataType: %d (Enum Text)", datatype);
				}
				
				if(data_len == 18)
				{
					int strlen = 8;
					char char_arr[strlen];
					int start = 6;
					
					for (int a = 0; a < strlen; a++) {
						if(start + a > 0 && start + a < data_len)
						{
							char_arr[a] = pdata[start+a];
						}
					}

					std::string s(char_arr, sizeof(char_arr));
										
					int tpos = start + strlen;
					
					float item_value = bytesToFloat(pdata[tpos+0],pdata[tpos+1],pdata[tpos+2],pdata[tpos+3]);
					
					ESP_LOGI("econet", "  %s: %f", s.c_str(), item_value);
				}
				else
				{
					ESP_LOGI("econet", "  Unexpected Write Data Length", datatype);	
				}
				
				// 01.01.00.07.00.00.4F.43.4F.4D.4D.41.4E.44.42.48.00.00
				// Object - OCOMMAND
				// 
			}
			else if(datatype == 2)
			{
				// ENUM TEXT
				
			}
			else if(datatype == 4)
			{
				// BYTES
				ESP_LOGI("econet", "  DataType: %d (Bytes)", datatype);
				
				//if(
			}
			else
			{
				ESP_LOGI("econet", "  DataType: %d", datatype);
			}
		}
		else if(class_type == 7)
		{
			// TIME AND DATA	
		}
		
		else if(class_type == 9)
		{
			// SYSTEM HANDLER - LISTING OF ADDRESSES ON BUS	
			// 09.01.00.00.03.80.00.00.03.40.00.00.05.00
			// 00 00 03 80
			// 00 00 03 40
			// 00 00 05 00
		}
		else
		{
			
		}
	}
	
	
	
	/*
	
	uint32_t UNKNOWN_HANDLER =  			241	;	// 80 00 00 F1
	uint32_t WIFI_MODULE =    				832	;	// 80 00 03 40
	uint32_t SMARTEC_TRANSLATOR = 			4160;	// 80 00 10 40
	uint32_t INTERNAL = 					4736; 	// 80 00 10 40
	uint32_t HEAT_PUMP_WATER_HEATER =       0x1280;
	uint32_t AIR_HANDLER = 					0x3c0;
	uint32_t CONTROL_CENTER = 				0x380;
	
	*/
	
	bool recognized = false;

	uint32_t exp_dst_adr = WIFI_MODULE;
	uint32_t exp_src_adr = SMARTEC_TRANSLATOR;
	int exp_msg_len = 115;
	
	if(type_id_ == 1)
	{
		exp_dst_adr = WIFI_MODULE;
		exp_src_adr = HEAT_PUMP_WATER_HEATER;
		exp_msg_len = 166;
		// Original was 66
		// New is 110
		// New new is 150 + 14 + 2 = 166
	}
	
	
	if(dst_adr == exp_dst_adr && src_adr == exp_src_adr)
	{
		if(command == ACK && pmsg_len == exp_msg_len)
		{
			recognized = false;

			int tpos = 0;
			uint8_t item_num = 1;

			while(tpos < data_len)
			{
				uint8_t item_len = pdata[tpos];
				uint8_t item_type = pdata[tpos+1] & 0x7F;

				if(item_type == 0)
				{
					float item_value = bytesToFloat(pdata[tpos+4],pdata[tpos+5],pdata[tpos+6],pdata[tpos+7]);

					if(type_id_ == 1)
					{
						if(item_num == 3)
						{
							setpoint = item_value;
						}
						else if(item_num == 4)
						{
							hot_water = item_value;
						}
						else if(item_num == 8)
						{
							ambient_temp = item_value;
						}
						else if(item_num == 9)
						{
							lower_water_heater_temp = item_value;
						}
						else if(item_num == 10)
						{
							upper_water_heater_temp = item_value;
						}
						else if(item_num == 11)
						{
							power_watt = item_value;
						}
						else if(item_num == 12)
						{
							evap_temp = item_value;
						}
						else if(item_num == 13)
						{
							suction_temp = item_value;
						}
						else if(item_num == 14)
						{
							discharge_temp = item_value;
						}
					}
				}
				else if(item_type == 2)
				{
					// Enumerated Text

					uint8_t item_value = pdata[tpos+4];

					uint8_t item_text_len = pdata[tpos+5];

					char char_arr[item_text_len];

					for (int a = 0; a < item_text_len; a++) {
						char_arr[a] = pdata[tpos+a+6];
					}

					std::string s(char_arr, sizeof(char_arr));

					if(type_id_ == 1)
					{
						if(item_num == 1)
						{
							if(item_value == 0)
							{
								enable_state = false;
							}
							else if(item_value == 1)
							{
								enable_state = true;
							}
						}
						else if(item_num == 2)
						{
							mode = static_cast<int>(item_value);
						}
						else if(item_num == 5)
						{
							if(item_value == 0) heatctrl = false;
							else heatctrl = true;
						}	
						else if(item_num == 6)
						{
							if(item_value == 0) fan_ctrl = false;
							else fan_ctrl = true;
						}
						else if(item_num == 7)
						{
							if(item_value == 0) comp_rly = false;
							else comp_rly = true;
						}
					}
				}
				tpos += item_len+1;
				item_num++;
			}

			instant_btus = std::max((float)((temp_out-temp_in)*flow_rate*8.334*60/0.92/1000),(float)0.0);
		}
		// publish_state(format_hex_pretty((const uint8_t *) buffer, msg_len));
	}
	else if(dst_adr == BROADCAST)
	{
		// Filter these for now
		recognized = false;
	}
	else if(src_adr == BROADCAST)
	{
		// Filter these for now
		recognized = false;
	}
	else if(dst_adr == SMARTEC_TRANSLATOR && src_adr == WIFI_MODULE)
	{
		// Filter these for now
	}

	if(recognized == false)
	{

		
		if(false)
		{
			if(command == 30)
			{
				// READ
				uint8_t class_type= pdata[0];
				ESP_LOGI("econet", "  class_type   : %d", class_type);

				if(class_type == 1)
				{
					char char_arr[data_len - 6];

					for (int a = 0; a < data_len - 6; a++) {
						char_arr[a] = pdata[a+4];
					}

					std::string s(char_arr, sizeof(char_arr));

					ESP_LOGD("econet", "  ValName : %s", s.c_str());
				}
				else if(class_type == 2)
				{
					int tpos = 0;
					uint8_t item_num = 1;

					while(tpos < data_len)
					{
						ESP_LOGD("econet", "  Item[%d] ", item_num);

						uint8_t item_len = pdata[tpos];
						uint8_t item_type = pdata[tpos+1] & 0x7F;

						ESP_LOGD("econet", "  ItemLen : %d", item_len);
						ESP_LOGD("econet", "  ItemType: %d", item_type);

						if(item_type == 0)
						{
							float item_value = bytesToFloat(pdata[tpos+4],pdata[tpos+5],pdata[tpos+6],pdata[tpos+7]);
							ESP_LOGD("econet", "  ItemVal : %f", item_value);
						}
						else if(item_type == 2)
						{
							// Enumerated Text

							uint8_t item_value = pdata[tpos+4];

							ESP_LOGD("econet", "  ItemVal : %d", item_value);

							uint8_t item_text_len = pdata[tpos+5];

							// uint8_t str_arr[item_text_len];
							char char_arr[item_text_len];

							for (int a = 0; a < item_text_len; a++) {
								// str_arr[a] = pdata[tpos+a+6];
								char_arr[a] = pdata[tpos+a+6];
							}

							std::string s(char_arr, sizeof(char_arr));

							ESP_LOGD("econet", "  ItemStr : %s", s.c_str());
						}
						tpos += item_len+1;
						item_num++;
					}
				}
			}
			else if(command == 6)
			{
				// ACK

				uint8_t class_type= pdata[0] & 0x7F;

				// 
				if(class_type == 0)
				{
					float item_value = bytesToFloat(pdata[11],pdata[12],pdata[13],pdata[14]);
					ESP_LOGD("econet", "  ItemVal : %f", item_value);
				}
				else if(class_type == 2)
				{
					int tpos = 0;
					uint8_t item_num = 1;

					while(tpos < data_len)
					{
						ESP_LOGD("econet", "  Item[%d] ", item_num);

						uint8_t item_len = pdata[tpos];
						uint8_t item_type = pdata[tpos+1] & 0x7F;

						ESP_LOGD("econet", "  ItemLen : %d", item_len);
						ESP_LOGD("econet", "  ItemType: %d", item_type);

						if(item_type == 0)
						{
							float item_value = bytesToFloat(pdata[tpos+4],pdata[tpos+5],pdata[tpos+6],pdata[tpos+7]);
							ESP_LOGD("econet", "  ItemVal : %f", item_value);
						}
						else if(item_type == 2)
						{
							// Enumerated Text

							uint8_t item_value = pdata[tpos+4];

							ESP_LOGD("econet", "  ItemVal : %d", item_value);

							uint8_t item_text_len = pdata[tpos+5];

							// uint8_t str_arr[item_text_len];
							char char_arr[item_text_len];

							for (int a = 0; a < item_text_len; a++) {
								// str_arr[a] = pdata[tpos+a+6];
								char_arr[a] = pdata[tpos+a+6];
							}

							// std::string item_string = reinterpret_cast<char *>(str_arr); 
							std::string s(char_arr, sizeof(char_arr));

							ESP_LOGD("econet", "  ItemStr : %s", s.c_str());
						}
						tpos += item_len+1;
						item_num++;
					}
				}
			}
		}
	}
	else
	{
		// ESP_LOGD("econet", "<<< %s", format_hex_pretty((const uint8_t *) buffer, msg_len).c_str());	
	}
// return true;
}
void Econet::read_buffer(int bytes_available) {

	if(bytes_available > 1200)
	{
		ESP_LOGI("econet", "BA=%d,LT=%d ms", bytes_available, this->act_loop_time_);
	}

	// Limit to Read 1200 bytes
	int bytes_to_read = std::min(bytes_available,1200);

	uint8_t bytes[bytes_to_read];

	uint32_t start_read_time = millis();
	// Read multiple bytes at the same time
	if(econet_uart->read_array(bytes,bytes_to_read) == false)
	{
		return; 
	}
    	ESP_LOGI("econet", "-- %s",format_hex_pretty((const uint8_t *) bytes, bytes_to_read).c_str());


	ESP_LOGI("econet", "read in ms: %d", millis() -start_read_time);

  // loop through and process all bytes.  note that a request/response may appear in the same buffer, so they are processed separately.
  // i refers to position of input buffer.  may contain 0x00's repeated.
  // pos refers to position in memory buffer of message.
	for(int i=0;i<bytes_to_read;i++)
	{
		uint8_t byte = bytes[i];
    bool override_pos_to_zero = false;
		buffer[pos] = byte;
 				
		if(pos == DST_ADR_POS || pos == SRC_ADR_POS)
		{
			if(byte != 0x80)
			{
				// Not the byte we are looking for
				// ESP_LOGI("econet", "<<< %s", format_hex_pretty((const uint8_t *) buffer, pos).c_str());
				// Not the byte we are looking for	
				// TODO
				// Loop through and check if we were off by a couple bytes
				override_pos_to_zero=true;
			}
		}
		else if(pos == LEN_POS)
		{
			data_len = byte;
			msg_len = data_len + MSG_HEADER_SIZE + MSG_CRC_SIZE;
		}
    pos++;
    if(override_pos_to_zero) pos=0;

		if(pos == msg_len && msg_len != 0 && pos != 0)
		{
  		ESP_LOGI("econet", "parse in ms: %d", millis() -start_read_time);
			// We have a full message.  This is where the message get parsed
			this->parse_rx_message();
			pos = 0;
			msg_len = 0;
			data_len = 0;
		}
		else if(pos == msg_len && msg_len != 0)
		{
			ESP_LOGD("econet", "This would have cuased problems");
		}
	}
	ESP_LOGI("econet", "loop in ms: %d", millis() -start_read_time);

}
void Econet::loop() {
	const uint32_t now = millis();
	bool flag = false;
	
	// Read Every 50ms After 10s afer boot
	if (now - this->last_read_ > 10) {
		this->act_loop_time_ = now - this->last_read_;
		this->last_read_ = now;
		// Read Everything that is in the buffer
		int bytes_available = econet_uart->available();

		if(bytes_available > 0)
		{
			this->last_read_data_ = now;
			ESP_LOGI("econet", "Read %d. ms=%d, lt=%d", bytes_available, now, act_loop_time_);
			flag = true;
			this->read_buffer(bytes_available);
		}
		else
		{
			// ESP_LOGI("econet", "--- millis()=%d", now);
		}
		if (now - this->last_read_data_ > 100)
		{
			// ESP_LOGI("econet", "request ms=%d", now);
			
			// Bus is Assumbed Available For Sending
			// This currently attempts a request every 1000ms
			if (now - this->last_request_ > 500)
			{
				ESP_LOGI("econet", "request ms=%d", now);
				this->last_request_ = now;
				this->make_request();
				req_id++;
				if(req_id > 1)
				{
					ESP_LOGI("econet", "request ms=%d", now);
					this->last_request_ = now;
					this->make_request();
					req_id++;
					if(req_id > 0)
					{
						req_id = 0;	
					}
				}
			}
			if(now - this->last_request_ > 10000 && type_id_ == 2)
			{
				if(false)
				{
					std::vector<uint8_t> data;
					
					// std::vector<uint8_t> req1 = {0x80,0x00,0x03,0x80,0x00,0x80,0x00,0x03,0x40,0x00,0x98,0x00,0x00,0x1E,0x02,0x01,0x00,0x00,0x41,0x55,0x54,0x4F,0x4D,0x4F,0x44,0x45,0x00,0x00,0x41,0x57,0x41,0x59,0x4D,0x4F,0x44,0x45,0x00,0x00,0x43,0x4F,0x4F,0x4C,0x53,0x45,0x54,0x50,0x00,0x00,0x44,0x45,0x41,0x44,0x42,0x41,0x4E,0x44,0x00,0x00,0x46,0x55,0x52,0x4E,0x47,0x46,0x41,0x4E,0x00,0x00,0x48,0x45,0x41,0x54,0x53,0x45,0x54,0x50,0x00,0x00,0x48,0x56,0x41,0x43,0x4D,0x4F,0x44,0x45,0x00,0x00,0x48,0x57,0x5F,0x47,0x5F,0x46,0x41,0x4E,0x00,0x00,0x52,0x45,0x4C,0x48,0x37,0x30,0x30,0x35,0x00,0x00,0x53,0x50,0x54,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x53,0x50,0x54,0x5F,0x53,0x54,0x41,0x54,0x00,0x00,0x53,0x54,0x41,0x54,0x4D,0x4F,0x44,0x45,0x00,0x00,0x53,0x54,0x41,0x54,0x4E,0x46,0x41,0x4E,0x00,0x00,0x53,0x54,0x41,0x54,0x5F,0x46,0x41,0x4E,0x00,0x00,0x56,0x41,0x43,0x53,0x54,0x41,0x54,0x45,0x54,0x06};
					
					// data = announce1;
					/*
					for(int i=0; i < data.size(); i++)
					{
						wbuffer[i] = data[i];
					}

					parse_tx_message();
					*/
					
				}
				if(false)
				{
					this->last_request_ = now;
					
					ESP_LOGI("econet", "request ms=%d", now);

					// do_once = true;
					// wbuffer

					std::vector<uint8_t> announce1 = {0x80,0x00,0x00,0xF1,0x00,0x80,0x00,0x03,0x80,0x00,0x0A,0x00,0x00,0x1F,0x08,0x06,0x0A,0x19,0x10,0x17,0x29,0x29,0x77,0xA9,0xAF,0xED};
					std::vector<uint8_t> announce2 = {0x80,0x00,0x03,0x80,0x00,0x80,0x00,0x00,0xF1,0x00,0x01,0x00,0x00,0x06,0x00,0x5E,0x9A};
					std::vector<uint8_t> announce3 = {0x80,0x00,0x00,0xF1,0x00,0x80,0x00,0x03,0x80,0x00,0x06,0x00,0x01,0x1F,0x09,0x01,0x00,0x00,0x03,0x80,0x7E,0xBB};
					
					std::vector<uint8_t> airhstat_req = {0x80,0x00,0x03,0xC0,0x00,0x80,0x00,0x03,0x80,0x00,0x0C,0x00,0x00,0x1E,0x01,0x01,0x00,0x00,0x41,0x49,0x52,0x48,0x53,0x54,0x41,0x54,0xA5,0x29};
					
					std::vector<uint8_t> prodmodn = {0x80,0x00,0x03,0xC0,0x00,0x80,0x00,0x03,0x80,0x00,0x0C,0x00,0x00,0x1E,0x01,0x00,0x00,0x00,0x50,0x52,0x4F,0x44,0x4D,0x4F,0x44,0x4E,0x2D,0x2C};
					
					std::vector<uint8_t> prodmodn_data = {0x01,0x00,0x00,0x00,0x50,0x52,0x4F,0x44,0x4D,0x4F,0x44,0x4E};
					
					std::vector<uint8_t> hwell_id = {0x80,0x00,0x01,0xC0,0x00,0x80,0x00,0x03,0x80,0x00,0x0C,0x00,0x00,0x1E,0x01,0x01,0x00,0x00,0x48,0x57,0x45,0x4C,0x4C,0x5F,0x49,0x44,0xA0,0xD5};
													  
					// 
					/*
					transmit_message(announce1);
					transmit_sync();
					
					delay(40);

					transmit_message(announce2);
					transmit_sync();
					
					delay(250);
					
					transmit_message(announce3);
					transmit_sync();
					
					delay(44);
					
					transmit_message(announce3);
					transmit_sync();
					
					delay(10);
					*/
					
					// std::vector<std::string> str_ids{};
						
					/*
					str_ids.push_back("AAUX1CFM");
					str_ids.push_back("AAUX2CFM");
					str_ids.push_back("AAUX3CFM");
					str_ids.push_back("AAUX4CFM");
					*/

					
					// str_ids.push_back("HWELL_ID");
					// str_ids.push_back("HWELMODL");
					// str_ids.push_back("HWCONFIG");
					// str_ids.push_back("HWSTATUS");
					// HWSTATUS
					// request_strings(0x1c0, 0x380, str_ids);
					
					// transmit_message(hwell_id);
					/*
					if(do_once == false && false)
					{
					
						do_once = true;
						
						transmit_message(hwell_id);

						transmit_sync();

						transmit_message(hwell_id);

						transmit_sync();

						transmit_message(hwell_id);

						transmit_sync();
						
					}							  
					
					econet_uart->flush();
					*/
				}
			}
		}
	}
}
void Econet::write_value(uint32_t dst_adr, uint32_t src_adr, std::string object, uint8_t type, float value)
{	
	std::vector<uint8_t> data;
	
	data.push_back(1);
	data.push_back(1);
	// Sometimes this is 2 and sometimes this is 0
	data.push_back(type);
	data.push_back(1);
	data.push_back(0);
	data.push_back(0);
	
	std::vector<uint8_t> sdata(object.begin(), object.end());
	
	for(int j=0; j<8;j++)
	{
		if(j < object.length())
		{
			data.push_back(sdata[j]);
		}
		else
		{
			data.push_back(0);
		}
	}
	
	uint32_t f_to_32 = floatToUint32(value);
	
	data.push_back((uint8_t)(f_to_32 >> 24));
	data.push_back((uint8_t)(f_to_32 >> 16));
	data.push_back((uint8_t)(f_to_32 >> 8));
	data.push_back((uint8_t)(f_to_32));
	
	transmit_message(dst_adr, src_adr, WRITE_COMMAND, data);
	
	
}
void Econet::request_strings(uint32_t dst_adr, uint32_t src_adr, std::vector<std::string> objects)
{	
	std::vector<uint8_t> data;
	
	int num_of_strs = objects.size();
	
	if(num_of_strs > 1)
	{
		// Read Class
		data.push_back(2);
	}
	else
	{
		data.push_back(1);
	}

	// Read Property
	data.push_back(1);

	int a = 0;

	for(int i=0; i<num_of_strs; i++)
	{
		data.push_back(0);
		data.push_back(0);

		std::string my_str = objects[i];

		std::vector<uint8_t> sdata(my_str.begin(), my_str.end());
		uint8_t *p = &sdata[0];
		
		for(int j=0; j<8;j++)
		{
			if(j < objects[i].length())
			{
				data.push_back(sdata[j]);
			}
			else
			{
				data.push_back(0);
			}
		}
	}
	transmit_message(dst_adr, src_adr, READ_COMMAND, data);
}
void Econet::transmit_message(uint32_t dst_adr, uint32_t src_adr, uint8_t command, std::vector<uint8_t> data)
{
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

	for(int i=0; i < wdata_len; i++)
	{
		wbuffer[14+i] = data[i];
	}

	uint16_t crc = gen_crc16(wbuffer,wdata_len + 14);

	wbuffer[wdata_len+14] = (uint8_t) crc;
	wbuffer[wdata_len+14+1] = (uint8_t) (crc >> 8);

	econet_uart->write_array(wbuffer,wdata_len+14+2);
	// econet_uart->flush();
	
	parse_tx_message();
}
void Econet::transmit_message(std::vector<uint8_t> data)
{
	for(int i=0; i < data.size(); i++)
	{
		wbuffer[i] = data[i];
	}

	econet_uart->write_array(wbuffer,data.size());
	// econet_uart->flush();
	
	parse_tx_message();
}
void Econet::transmit_sync()
{
	econet_uart->flush();
}
void Econet::send_datapoint(uint8_t datapoint_id, float value)
{
	for (auto &listener : this->listeners_) {
      if (listener.datapoint_id == datapoint_id)
        listener.on_datapoint(value);
    }	
}
void Econet::set_enable_state(bool state)
{
	if(state)
	{
		this->send_enable_disable = true;
		this->enable_disable_cmd = true;
	}
	else
	{
		this->send_enable_disable = true;
		this->enable_disable_cmd = false;
	}
}
void Econet::set_new_setpoint(float setpoint)
{
	send_new_setpoint = true;
	new_setpoint = setpoint;
}
void Econet::set_new_setpoint_low(float setpoint)
{
	send_new_setpoint_low = true;
	new_setpoint_low = setpoint;
}
void Econet::set_new_setpoint_high(float setpoint)
{
	send_new_setpoint_high = true;
	new_setpoint_high = setpoint;
}
void Econet::set_new_mode(float mode)
{
	send_new_mode = true;
	new_mode = mode;
}
void Econet::set_new_fan_mode(float fan_mode)
{
	send_new_fan_mode = true;
	new_fan_mode = fan_mode;
}
void Econet::set_new_dhumsetp(float setpoint)
{
	send_new_dhumsetp = true;
	new_dhumsetp = setpoint;
}
void Econet::dump_state() {
  
}
void Econet::register_listener(uint8_t datapoint_id, const std::function<void(float)> &func) {
  auto listener = DatapointListener{
      .datapoint_id = datapoint_id,
      .on_datapoint = func,
  };
  this->listeners_.push_back(listener);
}
	
}  // namespace econet
}  // namespace esphome
