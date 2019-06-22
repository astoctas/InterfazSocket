#include "stdafx.h"
#include "winuser.h"
#include "dc.h"

namespace interfaz {

	DC::DC(sio::client& _h, uint8_t _index) : h(_h), index(_index) {
		sio::message::ptr message = *(static_cast<sio::message::ptr*>(GlobalAlloc(GPTR, sizeof(sio::message::ptr*))));
	}

	DC::~DC() {}

	void DC::turnOn() {
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		message->get_map()["method"] = sio::string_message::create(std::string("on"));
		h.socket()->emit("OUTPUT", message );
	}

	void DC::turnOff() {
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		message->get_map()["method"] = sio::string_message::create(std::string("off"));
		h.socket()->emit("OUTPUT", message);
	}

	void DC::inverse() {
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		message->get_map()["method"] = sio::string_message::create(std::string("inverse"));
		h.socket()->emit("OUTPUT", message);
	}

	void DC::brake() {
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		message->get_map()["method"] = sio::string_message::create(std::string("brake"));
		h.socket()->emit("OUTPUT", message);
	}

	void DC::setDirection(uint8_t dir) {
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		message->get_map()["method"] = sio::string_message::create(std::string("direction"));
		message->get_map()["param"] = sio::int_message::create(dir);
		h.socket()->emit("OUTPUT", message);
	}

	void DC::setSpeed(uint8_t speed) {
		message = sio::object_message::create();
		message->get_map()["index"] = sio::int_message::create(index);
		message->get_map()["method"] = sio::string_message::create(std::string("power"));
		message->get_map()["param"] = sio::int_message::create(speed);
		h.socket()->emit("OUTPUT", message);
	}




}