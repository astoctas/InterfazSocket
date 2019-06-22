// dllmain.cpp : Define el punto de entrada de la aplicación DLL.
#include "stdafx.h"
#include "winuser.h"
// BOOST DEBUG
//#include "C:\Users\alejandro\Documents\Visual Studio 2017\boost_1_64_0\boost\system\config.hpp"
#include <boost/system/config.hpp>
#include "..\..\socket.io-client-cpp\src\sio_client.h"
#include <functional>
#include <thread>
#include <boost/thread/recursive_mutex.hpp>
#include <condition_variable>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>

#include "interfaz.h"

using namespace sio;
using namespace std;

#define SOCKET_URL "http://localhost:4268"

boost::recursive_mutex _lock;
std::condition_variable_any _cond;
bool connect_finish = false;

sio::client h;
std::vector<interfaz::Interfaz*> i;
int index = -1;



class connection_listener
{
	sio::client &handler;
public:
	connection_listener(sio::client& h) :
		handler(h)
	{
	}
	void on_connected()
	{
		std::cout << "sio connected " << std::endl;
		/* NOT USING MUTEX 
		_lock.lock();
		_cond.notify_all();
		connect_finish = true;
		_lock.unlock();
		*/
	}
	void on_close(client::close_reason const& reason)
	{
		std::cout << "sio closed " << reason << std::endl;
		//exit(0);
	}
	void on_fail()
	{
		std::cout << "sio failed " << std::endl;
		exit(0);
	}
};

#define DllExport __declspec( dllexport )

extern "C" {
	DllExport void WINAPI  connectSocket();
	DllExport void WINAPI  disconnectSocket();
	DllExport void WINAPI  restart();
	DllExport void WINAPI outputs(PCSTR data);
	DllExport void WINAPI output(uint8_t data);
	DllExport void WINAPI outputsOn();
	DllExport void WINAPI outputsOff();
	DllExport void WINAPI outputsReverse();
	DllExport void WINAPI outputsBrake();
	DllExport void WINAPI outputsDir(uint8_t dir);
	DllExport void WINAPI outputsSpeed(uint8_t speed);
	DllExport void WINAPI steppers(PCSTR data);
	DllExport void WINAPI stepper(uint8_t data);
	DllExport void WINAPI steppersSteps(int32_t steps);
	DllExport void WINAPI steppersStop();
	DllExport void WINAPI steppersDir(uint8_t dir);
	DllExport void WINAPI steppersSpped(uint16_t speed);
	DllExport WORD WINAPI steppersStatus(PBYTE Buffer);

	DllExport void WINAPI servos(PCSTR data);
	DllExport void WINAPI servo(uint8_t data);
	DllExport void WINAPI servosPosition(uint8_t pos);
	DllExport void WINAPI analogOn(uint8_t channel);
	DllExport void WINAPI analogOff(uint8_t channel);
	DllExport WORD WINAPI analogValue();
	DllExport WORD WINAPI device(PCSTR _name, PCSTR _device, PCSTR _options);
	DllExport void WINAPI deviceCall(PCSTR _name, PCSTR _method);
	DllExport void WINAPI deviceEvent(PCSTR _name, PCSTR _event, PCSTR _atts);
	DllExport PSTR WINAPI deviceGetValue(PCSTR _name, PCSTR _key);

	DllExport void WINAPI i2c(PCSTR name,uint8_t address, uint32_t delay);
	DllExport void WINAPI i2cReport(PCSTR name,uint16_t _register, uint32_t bytes);
	DllExport void WINAPI i2cRead(PCSTR name, uint16_t _register, uint32_t bytes);
	DllExport uint16_t WINAPI i2cValue(PCSTR name, uint16_t _register, PBYTE Buffer);
	DllExport void WINAPI i2cWrite(PCSTR name, uint16_t _register, PCSTR data);
	/*
	DllExport void WINAPI lcdWrite(uint8_t row, PCSTR data);
	DllExport void WINAPI lcdClear();
	*/
}

extern "C" BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		h.close();
		break;
	}

	return TRUE;
}


std::vector<uint8_t> PCSTR2ByteVector(PCSTR data) {
	std::string ss(data);
	std::vector<uint8_t> v;
	stringstream iss;

	if (ss.size()) {
		iss << ss;
		std::string n;
		while (iss >> n) {
			v.push_back(std::stoi(n, nullptr, 0));
		}
	}

	return v;
}



VOID WINAPI disconnectSocket() {
	h.sync_close();
}

VOID WINAPI restart() {
	h.socket()->emit("RESTART");
}

VOID WINAPI connectSocket() {
	
	connection_listener l(h);
	h.set_open_listener(std::bind(&connection_listener::on_connected, &l));
	h.set_close_listener(std::bind(&connection_listener::on_close, &l, std::placeholders::_1));
	h.set_fail_listener(std::bind(&connection_listener::on_fail, &l));

	h.connect(SOCKET_URL);
	interfaz::Interfaz* j = static_cast<interfaz::Interfaz*>(GlobalAlloc(GPTR, sizeof(interfaz::Interfaz)));
	index++;
	j = new interfaz::Interfaz(h, index);
	i.push_back(j);
	
	/* NOT USING MUTEX
	_lock.lock();
	if (!connect_finish)
	{
		_cond.wait(_lock);
	}
	_lock.unlock();
	*/
}

void WINAPI outputs(PCSTR data) {
	std::vector<uint8_t> v = PCSTR2ByteVector(data);
	for (int i = 0; i < v.size(); i++) std::cout << v.at(i);
	i.at(index)->setOutputs(v);
}


void WINAPI output(uint8_t data) {
	std::vector<uint8_t> v;
	v.push_back(data);
	//std::cout << v.at(0) << " - " << data;
	i.at(index)->setOutputs(v);
}



void WINAPI outputsOn() {
	i.at(index)->outputsOn();
}

void WINAPI outputsOff() {
	i.at(index)->outputsOff();
}

void WINAPI outputsReverse() {
	i.at(index)->outputsReverse();
}

void WINAPI outputsBrake() {
	i.at(index)->outputsBrake();
}

void WINAPI outputsDir(uint8_t dir) {
	i.at(index)->outputsDir(dir);
}

void WINAPI outputsSpeed(uint8_t speed) {
	i.at(index)->outputsSpeed(speed);
}

void WINAPI servos(PCSTR data) {
	std::vector<uint8_t> v = PCSTR2ByteVector(data);
	i.at(index)->setServos(v);
}

void WINAPI servo(uint8_t data) {
	std::vector<uint8_t> v;
	v.push_back(data);
	i.at(index)->setServos(v);
}

void WINAPI servosPosition(uint8_t pos) {
	i.at(index)->servosPosition(pos);
}

void WINAPI analogOn(uint8_t channel) {
	i.at(index)->setAnalog(channel, 1);
}

void WINAPI analogOff(uint8_t channel) {
	i.at(index)->setAnalog(channel, 0);
}

WORD WINAPI analogValue() {
	return i.at(index)->analogValue();
}



void WINAPI steppers(PCSTR data) {
	std::vector<uint8_t> v = PCSTR2ByteVector(data);
	i.at(index)->setSteppers(v);
}

void WINAPI stepper(uint8_t data) {
	std::vector<uint8_t> v;
	v.push_back(data);
	i.at(index)->setSteppers(v);
}


void WINAPI steppersSteps(int32_t steps) {
	i.at(index)->steppersSteps(steps);
}

void WINAPI steppersStop() {
	i.at(index)->steppersStop();
}

void WINAPI steppersDir(uint8_t dir) {
	i.at(index)->steppersDir(dir);
}

void WINAPI steppersSpped(uint16_t speed) {
	i.at(index)->steppersSpeed(speed);
}

WORD WINAPI steppersStatus(PBYTE Buffer) {
	std::vector<uint8_t> data = i.at(index)->steppersStatus();
	memcpy(Buffer, data.data(), data.size());
	return data.size();
}

WORD WINAPI device(PCSTR _name, PCSTR _device, PCSTR _options) {
	return i.at(index)->devicesCreate(_name, _device, _options);
}

void WINAPI deviceCall(PCSTR _name, PCSTR _method) {
	i.at(index)->deviceCall(_name, _method);
}

void WINAPI deviceEvent(PCSTR _name, PCSTR _event, PCSTR _atts) {
	i.at(index)->deviceEvent(_name, _event, _atts);
}

PSTR WINAPI deviceGetValue(PCSTR _name, PCSTR _key) {
	std::string ss = i.at(index)->deviceGetValue(_name, _key);
	size_t bufferSize = ss.size() + 1;
	char * str = static_cast<char*>(GlobalAlloc(GPTR, bufferSize));
	snprintf(str, bufferSize, "%s", ss.c_str());
	return str;
}



void WINAPI i2c(PCSTR name, uint8_t address) {
	i.at(index)->setI2C(name, address);
};

void WINAPI i2cReport(PCSTR name, uint16_t _register, uint32_t bytes) {
	i.at(index)->i2cReport(name, _register, bytes);
};

void WINAPI i2cRead(PCSTR name, uint16_t _register, uint32_t bytes) {
	i.at(index)->i2cRead(name, _register, bytes);
};


uint16_t WINAPI i2cValue(PCSTR name, uint16_t _register, PBYTE Buffer) {
	std::vector<uint8_t> data = i.at(index)->i2cValue(name, _register);
	memcpy(Buffer, data.data(), data.size());
	return data.size();
};


void WINAPI i2cWrite(PCSTR name, uint16_t _register, PCSTR data) {
	std::vector<uint8_t> v = PCSTR2ByteVector(data);
	i.at(index)->i2cWrite(name, _register, v);
};

/*
void WINAPI lcdWrite(uint8_t row, PCSTR data) {
	//i.at(index)->f->printLCD(row, data);
}

void WINAPI lcdClear() {
	//i.at(index)->f->clearLCD();
}

*/