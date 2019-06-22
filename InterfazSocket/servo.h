#pragma once

#ifndef		__SERVO_H_
#define		__SERVO_H_

#include "..\..\socket.io-client-cpp\src\sio_client.h"
#include "stdafx.h"
#include <windows.h>


namespace interfaz {

	class Servo
	{
	public:
		Servo(sio::client& _h, uint8_t _index);
		~Servo();
		void setPosition(uint8_t pos);
	private:
		sio::client &h;
		sio::message::ptr message;
		uint8_t index = 0;
	};

}

#endif