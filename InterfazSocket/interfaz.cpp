#include "stdafx.h"
#include "interfaz.h"
#include <iostream>
#include <boost/thread/recursive_mutex.hpp>

using namespace std;

boost::recursive_mutex device_message_mtx;
boost::recursive_mutex analog_message_mtx;
boost::recursive_mutex stepper_message_mtx;
boost::recursive_mutex i2c_message_mtx;

namespace interfaz {

	Interfaz::Interfaz(sio::client& _h, int _index)  : h(_h), index(_index){
		setConfig("MEGA");
		applyConfig();
		devicesReset();
		h.socket()->on("ANALOG_MESSAGE", sio::socket::event_listener_aux([&](string const& name, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp) {
			analog_message_mtx.lock();
			_analogs.at(data->get_map()["index"]->get_int())->setValue(data->get_map()["value"]->get_int());
			analog_message_mtx.unlock();
		}));
		h.socket()->on("I2C_MESSAGE", sio::socket::event_listener_aux([&](string const& name, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp) {
			//std::cout << data->get_map()["value"]->get_vector()[0]->get_int();
			i2c_message_mtx.lock();
			for (std::map<std::string, interfaz::I2C*>::iterator it = _i2c.begin(); it != _i2c.end(); ++it) {
				if ((it)->second->address == data->get_map()["address"]->get_int()) {
					(it)->second->setData(data->get_map()["register"]->get_int(), data->get_map()["value"]->get_vector());
				}
			}
			i2c_message_mtx.unlock();
		}));
		h.socket()->on("STEPPER_MESSAGE", sio::socket::event_listener_aux([&](string const& name, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp) {
			stepper_message_mtx.lock();
			//std::cout << data->get_map()["index"]->get_int() << ": " << data->get_map()["value"]->get_int();
			_steppers.at(data->get_map()["index"]->get_int())->setStatus(0);
			stepper_message_mtx.unlock();
		}));
		h.socket()->on("DEVICE_MESSAGE", sio::socket::event_listener_aux([&](string const& name, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp) {
			device_message_mtx.lock();
			try {
				if(data->get_map()["id"] != NULL && data->get_map()["data"] != NULL)
					_devices.find(data->get_map()["id"]->get_string())->second->setData(data->get_map()["data"]->get_map());
			}
			catch (int e) {
				std::cout << "device message catch error";
			}
			//std::cout << _devices.find(data->get_map()["id"]->get_string())->second->getValue("heading");
			//_devices.at(data->get_map()["id"]->get_int())->setData(data->get_map()["data"]->get_map());
			device_message_mtx.unlock();
		}));
		/* TEST */

	}

	Interfaz::~Interfaz() {}

	void Interfaz::setConfig(const char* model) {
		if (string(model) == "MEGA") {
			config.max_dc = 8;
			config.max_steppers = 3;
			config.max_servos = 3;
			config.max_analogs = 8;
			//config.servos.insert(config.servos.end(), { 10,11,12 });
			//config.inputs.insert(config.inputs.end(),{ 54,55,56,57,58,59,60,61 });
		}
	}


	void Interfaz::applyConfig() {
		for (int j = 0; j < config.max_dc; j++) {
			_dc.push_back(new interfaz::DC(h, j));
		}
		for (int j = 0; j < config.max_servos; j++) {
			_servos.push_back(new interfaz::Servo(h, j));
		}
		for (int j = 0; j < config.max_analogs; j++) {
			_analogs.push_back(new interfaz::AnalogInput(h, j));
		}
	
		for (int j = 0; j < config.max_steppers; j++) {
			_steppers.push_back(new interfaz::Stepper(h, j));
			//_steppers.at(j)->setSpeed(_steppers.at(j)->speed);
		}

		


	}

	void Interfaz::setOutputs(std::vector<uint8_t> _outputs) {
		outputs.assign(_outputs.begin(), _outputs.end());
	}
	
	void Interfaz::setSteppers(std::vector<uint8_t> _outputs) {
		steppers.assign(_outputs.begin(), _outputs.end());
	}

	void Interfaz::setServos(std::vector<uint8_t> _outputs) {
		servos.assign(_outputs.begin(), _outputs.end());
	}

	void Interfaz::outputsOn() {
		for (vector<uint8_t>::iterator it = outputs.begin(); it != outputs.end(); it++) {
			if(*it < _dc.size())
				_dc.at(*it)->turnOn();
		}
	}

	void Interfaz::outputsOff() {
		for (vector<uint8_t>::iterator it = outputs.begin(); it != outputs.end(); it++) {
			if (*it < _dc.size())
				_dc.at(*it)->turnOff();
		}
	}

	void Interfaz::outputsDir(uint8_t dir) {
		for (vector<uint8_t>::iterator it = outputs.begin(); it != outputs.end(); it++) {
			if (*it < _dc.size())
				_dc.at(*it)->setDirection(dir);
		}
	}
	
	void Interfaz::outputsSpeed(uint8_t speed) {
		for (vector<uint8_t>::iterator it = outputs.begin(); it != outputs.end(); it++) {
			if (*it < _dc.size())
				_dc.at(*it)->setSpeed(speed);
		}
	}
	
	void Interfaz::outputsBrake() {
		for (vector<uint8_t>::iterator it = outputs.begin(); it != outputs.end(); it++) {
			if (*it < _dc.size())
				_dc.at(*it)->brake();
		}
	}
	
	void Interfaz::outputsReverse() {
		for (vector<uint8_t>::iterator it = outputs.begin(); it != outputs.end(); it++) {
			if (*it < _dc.size())
				_dc.at(*it)->inverse();
		}
	}

	void Interfaz::servosPosition(uint8_t pos) {
		for (vector<uint8_t>::iterator it = servos.begin(); it != servos.end(); it++) {
			if (*it < _servos.size())
				_servos.at(*it)->setPosition(pos);
		}
	}

	void Interfaz::setAnalog(uint8_t channel, uint8_t enable) {
		analogs.clear();
		if(enable) analogs.push_back(channel);
		_analogs.at(channel -1)->reportAnalog(enable);
	}


	uint16_t Interfaz::analogValue() {
		analog_message_mtx.lock();
		if (analogs.size()) {
			for (vector<uint8_t>::iterator it = analogs.begin(); it != analogs.end(); it++) {
				if (*it < _analogs.size())
					return _analogs.at(*it - 1)->getValue();
			}

			//return _analogs.at(analogs.at(0))->getValue();
		}
		analog_message_mtx.unlock();
		return 0;
	}

	void Interfaz::steppersSteps(uint16_t _steps) {
		for (vector<uint8_t>::iterator it = steppers.begin(); it != steppers.end(); it++) {
			if (*it < _steppers.size())
				_steppers.at(*it)->steps(_steps);
		}
	}

	void Interfaz::steppersStop() {
		for (vector<uint8_t>::iterator it = steppers.begin(); it != steppers.end(); it++) {
			if (*it < _steppers.size())
				_steppers.at(*it)->stop();
		}
	}

	void Interfaz::steppersDir(uint8_t dir) {
		for (vector<uint8_t>::iterator it = steppers.begin(); it != steppers.end(); it++) {
			if (*it < _steppers.size())
				_steppers.at(*it)->setDirection(dir);
		}
	}

	void Interfaz::steppersSpeed(uint16_t speed) {
		for (vector<uint8_t>::iterator it = steppers.begin(); it != steppers.end(); it++) {
			if (*it < _steppers.size())
				_steppers.at(*it)->setSpeed(speed);
		}
	}

	
	std::vector<uint8_t> Interfaz::steppersStatus() {
		stepper_message_mtx.lock();
		std::vector<uint8_t> result;
		for (vector<uint8_t>::iterator it = steppers.begin(); it != steppers.end(); it++) {
			if (*it < _steppers.size())
				result.push_back(_steppers.at(*it)->getStatus());
		}
		stepper_message_mtx.unlock();
		return result;
	}

	void Interfaz::setI2C(std::string name, uint8_t address) {
		_i2c.insert(std::pair<std::string, interfaz::I2C*>(name, new interfaz::I2C(h, address)));
	}

	void Interfaz::i2cReport(std::string name, uint16_t _register, uint32_t bytes) {
		_i2c.find(name)->second->report(_register, bytes);
	}

	void Interfaz::i2cRead(std::string name, uint16_t _register, uint32_t bytes) {
		_i2c.find(name)->second->read(_register, bytes);
	}

	std::vector<uint8_t> Interfaz::i2cValue(std::string name, uint16_t _register) {
		i2c_message_mtx.lock();
		std::vector<uint8_t> res  =  _i2c.find(name)->second->getData(_register);
		i2c_message_mtx.unlock();
		return res;
	}

	void Interfaz::i2cWrite(std::string name, uint16_t _register, std::vector<uint8_t> data) {
		_i2c.find(name)->second->write(_register, data);
	}

	uint8_t Interfaz::devicesCreate(std::string _name, std::string _device, std::string _options) {
		_devices.insert(std::pair<std::string, interfaz::Device*>(_name, new interfaz::Device(h, _name, _device, _options)));
		return _devices.size();
	}
	
	void Interfaz::deviceCall(std::string _name, std::string _method) {
		_devices.find(_name)->second->call(_method);
	}

	void Interfaz::deviceEvent(std::string _name, std::string _event, std::string _atts) {
		_devices.find(_name)->second->setEvent(_event, _atts);
	}

	void Interfaz::devicesReset() {
		h.socket()->emit("DEVICES_RESET");
	}

	std::string Interfaz::deviceGetValue(std::string _name, std::string key) {
		device_message_mtx.lock();
		std::string str = _devices.find(_name)->second->getValue(key);
		device_message_mtx.unlock();
		return str;
	}




}