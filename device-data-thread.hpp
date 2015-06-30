#ifndef device-data-thread_HPP
	#include <nan.h>
	
	// Forward declaration. See node-mindwave.hpp.
	class Device;
	
	struct DeviceData {
		static bool update;
		static bool connecting;
		static std::string portName;
		
		static unsigned char poorSignal;
		static unsigned char attention;
		static unsigned char meditation;
		
		static void reset();
		
		static struct EEGData {
			static int delta;
			static int theta;
			static int lowAlpha;
			static int highAlpha;
			static int lowBeta;
			static int highBeta;
			static int lowGamma;
			static int midGamma;
		};
	};
	
	class DeviceDataThread : public NanAsyncWorker {
		public:
			DeviceDataThread(NanCallback* callback, Device* mindwaveDevice, bool readPorts): 
				NanAsyncWorker(callback), localDeviceReference(mindwaveDevice), readPorts(readPorts){};
			~DeviceDataThread();

			void Execute();
			void HandleOKCallback(); // Needs to callback with the DeviceData::update value.
			private: 
				Device* localDeviceReference;
				bool readPorts;
	};
	
#endif