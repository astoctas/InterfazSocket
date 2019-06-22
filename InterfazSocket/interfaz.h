#pragma once
#include "..\..\socket.io-client-cpp\src\sio_client.h"
#include "..\..\socket.io-client-cpp\src\sio_message.h"
#include "stdafx.h"
#include "dc.h"
#include "servo.h"
#include "analogInput.h"
#include "stepper.h"
#include "device.h"
#include "i2c.h"

using namespace std;

namespace interfaz {

	class DC;
	class Servo;
	class Stepper;
	class AnalogInput;
	class Device;
	class I2C;

	class Interfaz
	{
	public:
		Interfaz(sio::client& _h, int _index);
		~Interfaz();

		void applyConfig();
		void setConfig(const char* model);
		void setOutputs(std::vector<uint8_t> _outputs);
		void setSteppers(std::vector<uint8_t> _outputs);
		void setServos(std::vector<uint8_t> _outputs);
		void outputsOn();
		void outputsOff();
		void outputsDir(uint8_t dir);
		void outputsSpeed(uint8_t speed);
		void outputsBrake();
		void outputsReverse();
		void servosPosition(uint8_t pos);
		void setAnalog(uint8_t channel, uint8_t enable);
		uint16_t analogValue();
		void steppersSteps(uint16_t _steps);
		void steppersStop();
		void steppersDir(uint8_t dir);
		void steppersSpeed(uint16_t speed);
		std::vector<uint8_t> steppersStatus();

		uint8_t devicesCreate(std::string _name, std::string _device, std::string _options);
		void devicesReset();
		void deviceCall(std::string _name, std::string _method);
		void deviceEvent(std::string _name, std::string _event, std::string _atts);
		std::string deviceGetValue(std::string _name, std::string key);
		

		void setI2C(std::string name, uint8_t address);
		void i2cReport(std::string name, uint16_t _register, uint32_t bytes);
		void i2cRead(std::string name, uint16_t _register, uint32_t bytes);
		std::vector<uint8_t> i2cValue(std::string name, uint16_t _register);
		void i2cWrite(std::string name, uint16_t _register, std::vector<uint8_t> data);
		/*
		void i2cWrite(std::string name, std::vector<uint8_t> data);
		*/

	private:
		sio::client &h;
		sio::message::ptr message;
		int index = 0;
		std::vector<uint8_t> outputs, steppers, servos, analogs;

		std::vector<interfaz::DC*> _dc;
		std::vector<interfaz::Servo*> _servos;
		std::vector<interfaz::AnalogInput*> _analogs;
		std::vector<interfaz::Stepper*> _steppers;
		std::map<std::string, interfaz::Device*> _devices;
		std::map<std::string, interfaz::I2C*> _i2c;

		struct _config {
			int max_dc;
			int max_steppers;
			int max_servos;
			int max_analogs;
		} config;


	};

}