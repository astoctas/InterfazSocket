#include "stdafx.h"
#include "winuser.h"
#include "i2c.h"
#include <iostream>

namespace interfaz {

	I2C::I2C(sio::client& _h, uint8_t _address) : h(_h), address(_address) {
		sio::message::ptr message = *(static_cast<sio::message::ptr*>(GlobalAlloc(GPTR, sizeof(sio::message::ptr*))));
	}

	I2C::~I2C() {}

	void I2C::report(uint16_t _register, uint32_t bytes) 
	{
		message = sio::object_message::create();
		message->get_map()["address"] = sio::int_message::create(address);
		message->get_map()["method"] = sio::string_message::create(std::string("on"));
		message->get_map()["register"] = sio::int_message::create(_register);
		message->get_map()["param"] = sio::int_message::create(bytes);
		h.socket()->emit("I2C", message);
	}

	void I2C::read(uint16_t _register, uint32_t bytes)
	{
		message = sio::object_message::create();
		message->get_map()["address"] = sio::int_message::create(address);
		message->get_map()["method"] = sio::string_message::create(std::string("read"));
		message->get_map()["register"] = sio::int_message::create(_register);
		message->get_map()["param"] = sio::int_message::create(bytes);
		h.socket()->emit("I2C", message);
	}

	void I2C::write(uint16_t _register, std::vector<uint8_t> _data)
	{
		message = sio::object_message::create();
		message->get_map()["address"] = sio::int_message::create(address);
		message->get_map()["method"] = sio::string_message::create(std::string("write"));
		message->get_map()["register"] = sio::int_message::create(_register);

		sio::message::ptr arrData = sio::array_message::create();
		// FOR
		for (std::vector<uint8_t>::iterator it = _data.begin(); it != _data.end(); ++it) {
			arrData->get_vector().push_back(sio::int_message::create(*it));
		}
		message->get_map()["param"] = arrData;
		h.socket()->emit("I2C", message);
	}


	void I2C::setData(uint16_t _register, std::vector<sio::message::ptr> _data)
	{
		std::vector<uint8_t> tmp;
		for (std::vector<sio::message::ptr>::iterator it = _data.begin(); it != _data.end(); ++it) {
			tmp.push_back((*it)->get_int());
			std::cout << (*it)->get_int();
		}
		data[_register] = tmp;
	}

	std::vector<uint8_t> I2C::getData(uint16_t _register) {
		if (data.find(_register) == data.end()) {
			std::vector<uint8_t> empty;
			return empty;
		}
		else {
			return data.find(_register)->second;
		}
	}

}