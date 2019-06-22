#pragma once

#ifndef		__STEPPER_H_
#define		__STEPPER_H_

#include "..\..\socket.io-client-cpp\src\sio_client.h"
#include "stdafx.h"
#include <windows.h>


namespace interfaz {

	class Stepper
	{
	public:
		Stepper(sio::client& _h, uint8_t _index);
		~Stepper();
		void steps(uint16_t _steps);
		void stop();
		void setDirection(uint8_t dir);
		void setSpeed(uint16_t speed);
		void setStatus(uint8_t _status);
		uint8_t getStatus();
	private:
		sio::client &h;
		sio::message::ptr message;
		uint8_t index = 0;
		uint8_t direction = 0;
		uint8_t status = 0;
	};

}

#endif
