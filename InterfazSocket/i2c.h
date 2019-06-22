#pragma once

#ifndef		__I2C_H_
#define		__I2C_H_

#include "..\..\socket.io-client-cpp\src\sio_client.h"
#include "stdafx.h"
#include <windows.h>


namespace interfaz {

	class I2C
	{
	public:
		I2C(sio::client& _h, uint8_t _index);
		~I2C();
		void report(uint16_t _register, uint32_t bytes);
		void read(uint16_t _register, uint32_t bytes);
		void setData(uint16_t _register, std::vector<sio::message::ptr> _data);
		std::vector<uint8_t> getData(uint16_t _register);
		void write(uint16_t _register, std::vector<uint8_t> _data);
		uint8_t address = 0;
	private:
		sio::client &h;
		sio::message::ptr message;
		std::map<uint8_t, std::vector<uint8_t>> data;
	};

}

#endif
