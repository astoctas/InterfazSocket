#pragma once

#ifndef		__ANALOG_INPUT_H_
#define		__ANALOG_INPUT_H_

#include "..\..\socket.io-client-cpp\src\sio_client.h"
#include "stdafx.h"
#include <windows.h>


namespace interfaz {

	class AnalogInput
	{
	public:
		AnalogInput(sio::client& _h, uint8_t _index);
		~AnalogInput();
		void reportAnalog(uint8_t enable);
		uint16_t getValue();
		void setValue(uint16_t _value);
	private:
		sio::client &h;
		sio::message::ptr message;
		uint8_t index = 0;
		uint16_t value = 0;
	};

}

#endif
