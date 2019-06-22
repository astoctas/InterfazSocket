#pragma once

#ifndef		__DEVICE_H_
#define		__DEVICE_H_

#include "..\..\socket.io-client-cpp\src\sio_client.h"
#include "..\..\socket.io-client-cpp\src\sio_message.h"
#include "stdafx.h"
#include <windows.h>


namespace interfaz {

	class Device
	{
	public:
		Device(sio::client& _h, std::string _name, std::string _device, std::string _options);
		~Device();
		void call(std::string _method);
		void setEvent(std::string _event, std::string _atts);
		void setData(std::map<std::string, sio::message::ptr> _data);
		std::string getValue(std::string _key);
	private:
		sio::client &h;
		sio::message::ptr message;
		std::string name = "";
		std::map<std::string, sio::message::ptr> data;
	};

}

#endif