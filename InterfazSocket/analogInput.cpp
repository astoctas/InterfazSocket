#include "stdafx.h"
#include "winuser.h"
#include "analogInput.h"

namespace interfaz {

	AnalogInput::AnalogInput(sio::client& _h, uint8_t _index) : h(_h), index(_index) {
		sio::message::ptr message = *(static_cast<sio::message::ptr*>(GlobalAlloc(GPTR, sizeof(sio::message::ptr*))));
	}

	AnalogInput::~AnalogInput() {}

	uint16_t AnalogInput::getValue() {
		return value;
	}

	void AnalogInput::setValue(uint16_t _value) {
		value = _value;
	}

	void AnalogInput::reportAnalog(uint8_t enable) {
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		if (enable) {
			message->get_map()["method"] = sio::string_message::create(std::string("on"));
		}
		else {
			message->get_map()["method"] = sio::string_message::create(std::string("off"));
		}
		h.socket()->emit("ANALOG", message);
	}


}