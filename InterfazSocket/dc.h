#pragma once

#ifndef		__DC_H_
#define		__DC_H_

#include "..\..\socket.io-client-cpp\src\sio_client.h"
#include "stdafx.h"
#include <windows.h>


namespace interfaz {

	class DC
	{
	public:
		DC(sio::client& _h, uint8_t _index);
		~DC();
		void turnOn();
		void turnOff();
		void brake();
		void inverse();
		void setDirection(uint8_t dir);
		void setSpeed(uint8_t speed);
	private:
		sio::client &h;
		sio::message::ptr message;
		uint8_t index = 0;
	};

}

#endif