#ifndef device_HPP
	#include <nan.h>
	#include "serial/serial.h"	
	
	// A super simple class to wrap the struct/class from the serial port library.
	class Device: public node::ObjectWrap {
		public:
			Device();
			~Device();
			
			// Bolier plate.
			static v8::Persistent<v8::FunctionTemplate> constructor;
			static void initialise(v8::Handle<v8::Object> exports); // Main...	
			
			// Javascript constructor.
			static NAN_METHOD(New);
			
			// Class methods
			static NAN_METHOD(openDevice);
			static NAN_METHOD(pollDevice);
			static NAN_METHOD(closeDevice);
			
			// Readonly property getters.
			static NAN_GETTER(getSignal);
			static NAN_GETTER(getMeditation);
			static NAN_GETTER(getAttention);
			
			static NAN_GETTER(getUpdate);
			static NAN_GETTER(getConnectionStatus);
			static NAN_GETTER(getPortName);
			static NAN_GETTER(getDelta);
			static NAN_GETTER(getTheta);
			static NAN_GETTER(getLowAlpha);
			static NAN_GETTER(getHighAlpha);
			static NAN_GETTER(getLowBeta);
			static NAN_GETTER(getHighBeta);
			static NAN_GETTER(getLowGamma);
			static NAN_GETTER(getMidGamma);
						
			static unsigned int byteValue(const unsigned char* buffer, int index);
			
			// Device parser method callback.
			static void parseData(
				unsigned char extendedCodeLevel,
				unsigned char code,
				unsigned char valueLength,
				const unsigned char* value,
				void* customData 				
			);
			
			void workerThreadDelegate(NanCallback* callback, Device* device, bool readPortFlag);
			void readDevice(Device* device);
			void readPorts(Device* device);
			
			private:
				serial::Serial object;
				std::string portName;
	};
#endif