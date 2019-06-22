#include "stdafx.h"
#include "winuser.h"
#include "servo.h"

namespace interfaz {

	Servo::Servo(sio::client& _h, uint8_t _index) : h(_h), index(_index) {
		sio::message::ptr message = *(static_cast<sio::message::ptr*>(GlobalAlloc(GPTR, sizeof(sio::message::ptr*))));
	}

	Servo::~Servo() {}

	void Servo::setPosition(uint8_t pos) {
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		message->get_map()["method"] = sio::string_message::create(std::string("position"));
		message->get_map()["param"] = sio::int_message::create(pos);
		h.socket()->emit("SERVO", message);
	}
}