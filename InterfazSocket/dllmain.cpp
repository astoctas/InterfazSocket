// dllmain.cpp : Define el punto de entrada de la aplicación DLL.
#include "stdafx.h"
#include "winuser.h"
#include "..\..\socket.io-client-cpp\src\sio_client.h"
// BOOST DEBUG
//#include "C:\Users\alejandro\Documents\Visual Studio 2017\boost_1_64_0\boost\system\config.hpp"
#include <boost/system/config.hpp>


#include <functional>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

#ifdef WIN32
#define HIGHLIGHT(__O__) std::cout<<__O__<<std::endl
#define EM(__O__) std::cout<<__O__<<std::endl
#include <stdio.h>
#include <tchar.h>
#define MAIN_FUNC int _tmain(int argc, _TCHAR* argv[])
#else
#define HIGHLIGHT(__O__) std::cout<<"\e[1;31m"<<__O__<<"\e[0m"<<std::endl
#define EM(__O__) std::cout<<"\e[1;30;1m"<<__O__<<"\e[0m"<<std::endl
#define MAIN_FUNC int main(int argc ,const char* args[])
#endif
using namespace sio;
using namespace std;
std::mutex _lock;
std::condition_variable_any _cond;
bool connect_finish = false;

sio::client h;


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
	DllExport VOID __stdcall  connectSocket();
	DllExport VOID __stdcall  disconnectSocket();
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

VOID __stdcall disconnectSocket() {
	h.sync_close();
}

VOID __stdcall connectSocket() {
	
	connection_listener l(h);
	h.set_open_listener(std::bind(&connection_listener::on_connected, &l));
	h.set_close_listener(std::bind(&connection_listener::on_close, &l, std::placeholders::_1));
	h.set_fail_listener(std::bind(&connection_listener::on_fail, &l));

	h.connect("http://localhost:4268");
	
	/* NOT USING MUTEX
	_lock.lock();
	if (!connect_finish)
	{
		_cond.wait(_lock);
	}
	_lock.unlock();
	*/
}