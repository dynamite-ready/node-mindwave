#include <nan.h>
#include <iostream>
#include <device.hpp>
#include "serial/serial.h"
#include <device-data-thread.hpp>
#include <ThinkGearStreamParser.h>

// Why can't I just use "std"?
using std::string;
using std::vector;
using std::endl;
using std::cout;

using namespace v8;
using namespace node;

// The Javascript based constructor wrapper.
Persistent<FunctionTemplate> Device::constructor;

// Native object constructor. Effectively empty here.
Device::Device(){}
Device::~Device(){}

NAN_METHOD(Device::New){
	NanScope();
    Device* device = new Device();
    device -> Wrap(args.This());
    NanReturnValue(args.This());
} 

unsigned int Device::byteValue(const unsigned char* buffer, int index){
	return (
		((unsigned int) buffer[((index * 3) - 2) - 1] << 16) | 
		((unsigned int) buffer[((index * 3) - 2)] << 8) | 
		(unsigned int) buffer[((index * 3) - 2) + 1]
	);	
}

void Device::readPorts(Device* device){
	vector<serial::PortInfo> devices_found = serial::list_ports();
	vector<serial::PortInfo>::iterator iter = devices_found.begin();
	
	try{
		while(iter != devices_found.end()){
			serial::PortInfo serialDevice = *iter++;
			
			device -> object.setPort(serialDevice.port.c_str());
			device -> object.open();
			device -> object.setTimeout(serial::Timeout::simpleTimeout(1000)); // Make this value a macro, I guess.
			DeviceData::portName = serialDevice.description;
			
			//cout << "Is the serial port open?";
			if(device -> object.isOpen()){
				DeviceData::update = true;
				DeviceData::connecting = true;
			}
		}
	} catch(...) {
		DeviceData::reset();			
	}		
}

void Device::readDevice(Device* device){
	// Initialize ThinkGear stream parser.
	ThinkGearStreamParser parser;
	int initParser = THINKGEAR_initParser(
		&parser, 
		PARSER_TYPE_PACKETS, 
		Device::parseData, 
		NULL
	);
	
	// Start polling kick off the infinite device stream loop.
	try{
		while(DeviceData::update){
			string result = device -> object.read(); // Is usually a string.
			// Convert the stream to a type that the parser can work with.
			// if(result.length() == 0) throw std::runtime_error("Bad packet.");
			unsigned char byte = result[0];
			// Parse the stream.
			int stream = THINKGEAR_parseByte(&parser, byte);
			if(!DeviceData::update) throw std::runtime_error("Bad packet.");
			// if(stream < 0) throw std::runtime_error("Bad packet.");
		}
	} catch(...){
		DeviceData::reset();
	}
	
	DeviceData::reset();
}

void Device::parseData(
	unsigned char extendedCodeLevel,
	unsigned char code,
	unsigned char valueLength,
	const unsigned char* value,
	void* customData 
){
	// Parse Shenanigans...
	try{
		if(extendedCodeLevel == 0){
			switch(code){
				case(0xD0): DeviceData::connecting = false; break; 
				case(0xD1): DeviceData::update = false; break; 
				case(0xD2): DeviceData::update = false; break; 
				case(0xD3): DeviceData::update = false; break;
				// case(0x00): DeviceData::update = false; break; // Hmmm...
				
				case(0x02):	DeviceData::poorSignal = (value[0] & 0xFF); break;			
				case(0x04): DeviceData::attention = (value[0] & 0xFF); break;
				case(0x05): DeviceData::meditation = (value[0] & 0xFF); break;
				
				case(0x83):
					DeviceData::EEGData::delta = Device::byteValue(value, 1);
					DeviceData::EEGData::theta = Device::byteValue(value, 2);
					DeviceData::EEGData::lowAlpha = Device::byteValue(value, 3);
					DeviceData::EEGData::highAlpha = Device::byteValue(value, 4);
					DeviceData::EEGData::lowBeta = Device::byteValue(value, 5);
					DeviceData::EEGData::highBeta = Device::byteValue(value, 6);
					DeviceData::EEGData::lowGamma = Device::byteValue(value, 7);
					DeviceData::EEGData::midGamma = Device::byteValue(value, 8);
				break;
				
				// default: DeviceData::update = false;
			}
		}
	} catch(...){
		DeviceData::reset();
	}	
}

void Device::workerThreadDelegate(NanCallback* callback, Device* device, bool readPortFlag){
	NanAsyncWorker* spinWorkerThread = new DeviceDataThread(callback, device, readPortFlag);
	NanAsyncQueueWorker(spinWorkerThread);
}

NAN_METHOD(Device::openDevice){
	NanScope();	
	
	Device* device = ObjectWrap::Unwrap<Device>(args.This());
	NanCallback* callback = new NanCallback(args[0].As<Function>());
	device -> workerThreadDelegate(callback, device, true);
	
	NanReturnValue(NanNew<Boolean>(false));
}

NAN_METHOD(Device::pollDevice){
	NanScope();	
	
	Device* device = ObjectWrap::Unwrap<Device>(args.This());
	// This call back should run in an infinite loop...
	NanCallback* callback = new NanCallback(args[0].As<Function>());
	device -> workerThreadDelegate(callback, device, false);
	
	NanReturnValue(NanNew<Boolean>(false));
}

NAN_METHOD(Device::closeDevice){
	NanScope();
	
	Device* device = ObjectWrap::Unwrap<Device>(args.This());
	device -> object.close();
	device -> ~Device();
	
	NanReturnValue(NanNew<Boolean>(false));
}

// Getters...
NAN_GETTER(Device::getPortName){
	NanScope();
	Handle<String> value = NanNew<String>(DeviceData::portName);
	NanReturnValue(value);
}

NAN_GETTER(Device::getUpdate){
	NanScope();
	Handle<Boolean> value = NanNew<Boolean>(DeviceData::update); // Less than an int...
	NanReturnValue(value);
}

NAN_GETTER(Device::getConnectionStatus){
	NanScope();
	Handle<Boolean> value = NanNew<Boolean>(DeviceData::connecting); // Less than an int...
	NanReturnValue(value);
}

NAN_GETTER(Device::getSignal){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::poorSignal); // Less than an int...
	NanReturnValue(value);
}

NAN_GETTER(Device::getAttention){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::attention); // Less than an int...
	NanReturnValue(value);
}

NAN_GETTER(Device::getMeditation){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::meditation); // Less than an int...
	NanReturnValue(value);
}

NAN_GETTER(Device::getDelta){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::EEGData::delta);
	NanReturnValue(value);
}

NAN_GETTER(Device::getTheta){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::EEGData::theta);
	NanReturnValue(value);
}

NAN_GETTER(Device::getLowAlpha){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::EEGData::lowAlpha);
	NanReturnValue(value);
}

NAN_GETTER(Device::getHighAlpha){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::EEGData::highAlpha);
	NanReturnValue(value);
}

NAN_GETTER(Device::getLowBeta){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::EEGData::lowBeta);
	NanReturnValue(value);
}

NAN_GETTER(Device::getHighBeta){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::EEGData::highBeta);
	NanReturnValue(value);
}

NAN_GETTER(Device::getLowGamma){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::EEGData::lowGamma);
	NanReturnValue(value);
}

NAN_GETTER(Device::getMidGamma){
	NanScope();
	Handle<Integer> value = NanNew<Integer>(DeviceData::EEGData::midGamma);
	NanReturnValue(value);
}

void Device::initialise(Handle<Object> exports){
	NanScope();
	
	Local<String> name = NanNew<String>("Mindwave");
	// Specify and create the Javascript based object constructor.
    Local<FunctionTemplate> constructorTemplate = NanNew<FunctionTemplate>(Device::New);
    constructorTemplate -> InstanceTemplate() -> SetInternalFieldCount(1); // Brr...
    constructorTemplate -> SetClassName(name);
	
	NanAssignPersistent(Device::constructor, constructorTemplate);
	
	// Create the native object prototype.
	Local<ObjectTemplate> proto = constructorTemplate -> PrototypeTemplate();
		proto -> SetInternalFieldCount(1);
		proto -> Set(NanNew<String>("open"), NanNew<FunctionTemplate>(Device::openDevice) -> GetFunction());
		proto -> Set(NanNew<String>("poll"), NanNew<FunctionTemplate>(Device::pollDevice) -> GetFunction());
		proto -> Set(NanNew<String>("close"), NanNew<FunctionTemplate>(Device::closeDevice) -> GetFunction());
		proto -> SetAccessor(NanNew<String>("running"), getUpdate);
		proto -> SetAccessor(NanNew<String>("connecting"), getConnectionStatus);
		proto -> SetAccessor(NanNew<String>("port"), getPortName);
	
		Local<Object> yield = NanNew<Object>();
			//yield -> SetInternalFieldCount(4);
			yield -> SetAccessor(NanNew<String>("POOR_SIGNAL"), getSignal);
			yield -> SetAccessor(NanNew<String>("ATTENTION"), getAttention);
			yield -> SetAccessor(NanNew<String>("MEDITATION"), getMeditation);
			
			Local<Object> yieldEEG = NanNew<Object>();
				//yieldEEG -> SetInternalFieldCount(8);
				yieldEEG -> SetAccessor(NanNew<String>("Delta"), getDelta);
				yieldEEG -> SetAccessor(NanNew<String>("Theta"), getTheta);
				yieldEEG -> SetAccessor(NanNew<String>("Low-Alpha"), getLowAlpha);
				yieldEEG -> SetAccessor(NanNew<String>("High-Alpha"), getHighAlpha);
				yieldEEG -> SetAccessor(NanNew<String>("Low-Beta"), getLowBeta);
				yieldEEG -> SetAccessor(NanNew<String>("High-Beta"), getHighBeta);
				yieldEEG -> SetAccessor(NanNew<String>("Low-Gamma"), getLowGamma);
				yieldEEG -> SetAccessor(NanNew<String>("Mid-Gamma"), getMidGamma);
				
			yield -> Set(NanNew<String>("ASIC_EEG_POWER_INT"), yieldEEG);	
		
		proto -> Set(NanNew<String>("data"), yield);		
	
	// Export the composed persistent container.
	exports -> Set(name, constructorTemplate -> GetFunction());		
}