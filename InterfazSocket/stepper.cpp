#include "stdafx.h"
#include "winuser.h"
#include "stepper.h"

namespace interfaz {

	Stepper::Stepper(sio::client& _h, uint8_t _index) : h(_h), index(_index) {
		sio::message::ptr message = *(static_cast<sio::message::ptr*>(GlobalAlloc(GPTR, sizeof(sio::message::ptr*))));
	}

	Stepper::~Stepper() {}

	void Stepper::steps(uint16_t pos) {
		int32_t steps;
		steps = pos;
		if (direction) {
			steps = steps * -1;
		}
		status = 1;
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		message->get_map()["method"] = sio::string_message::create(std::string("steps"));
		message->get_map()["param"] = sio::int_message::create(steps);
		h.socket()->emit("STEPPER", message);
	}

	void Stepper::stop() {
		status = 0;
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		message->get_map()["method"] = sio::string_message::create(std::string("stop"));
		h.socket()->emit("STEPPER", message);
	}

	void Stepper::setSpeed(uint16_t speed) {
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		message->get_map()["method"] = sio::string_message::create(std::string("speed"));
		message->get_map()["param"] = sio::int_message::create(speed);
		h.socket()->emit("STEPPER", message);
	}

	void Stepper::setDirection(uint8_t dir) {
		direction = dir;
	}

	void Stepper::setStatus(uint8_t _status) {
		status = _status;
	}

	uint8_t Stepper::getStatus() {
		return status;
	}


}