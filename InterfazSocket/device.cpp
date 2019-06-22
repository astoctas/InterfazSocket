#include "stdafx.h"
#include "winuser.h"
#include "device.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace interfaz {

	Device::Device(sio::client& _h, std::string _name, std::string _device, std::string _options) : h(_h), name(_name) {
		sio::message::ptr message = *(static_cast<sio::message::ptr*>(GlobalAlloc(GPTR, sizeof(sio::message::ptr*))));
		message = sio::object_message::create();
		message->get_map()["id"] = sio::string_message::create(std::string(_name));
		message->get_map()["device"] = sio::string_message::create(std::string(_device));
		message->get_map()["options"] = sio::string_message::create(std::string(_options));
		h.socket()->emit("DEVICE", message, [&](sio::message::list const& msg) {

		});
	}

	Device::~Device() {}
	
	void Device::call(std::string _method) {
		message = sio::object_message::create();
		message->get_map()["id"] = sio::string_message::create(std::string(name));
		message->get_map()["method"] = sio::string_message::create(std::string(_method));
		h.socket()->emit("DEVICE_CALL", message, [&](sio::message::list const& msg) {

		});
	}

	void Device::setEvent(std::string _event, std::string _atts) {
		message = sio::object_message::create();
		message->get_map()["id"] = sio::string_message::create(std::string(name));
		message->get_map()["event"] = sio::string_message::create(std::string(_event));
		std::vector<std::string> atts;
		boost::split(atts, _atts, boost::is_any_of(","));
		sio::message::ptr arrAtts = sio::array_message::create();
		
		// FOR
		for (std::vector<std::string>::iterator it = atts.begin(); it != atts.end(); ++it) {
			arrAtts->get_vector().push_back(sio::string_message::create(*it));
		}

		message->get_map()["attributes"] = arrAtts;
		h.socket()->emit("DEVICE_EVENT", message, [&](sio::message::list const& msg) {

		});
	}


	void Device::setData(std::map<std::string, sio::message::ptr> _data) {
		data = _data;
	}

	std::string Device::getValue(std::string _key) {
		try {

			if(data.find(_key)->second != NULL)
			switch (data.find(_key)->second->get_flag()) {
			case sio::message::flag_integer:
				return std::to_string(data.find(_key)->second->get_int());
				break;
			case sio::message::flag_double: 
					return std::to_string(data.find(_key)->second->get_double());
					break;
			case sio::message::flag_boolean:
				return std::to_string(data.find(_key)->second->get_bool());
				break;
			case sio::message::flag_string:
				return data.find(_key)->second->get_string();
				break;
			default: 
				return "";
			}
		
		}
		catch (int e) {

			std::cout << "catch deviceGetValue";
			return "";
		}
		return "";
		/* TODO VER CASOS DE OBJETOS */
	}
	
}