#include <nan.h>
#include <device.hpp>
#include <device-data-thread.hpp>

bool DeviceData::update = false;
bool DeviceData::connecting = false;
std::string DeviceData::portName = "none";

unsigned char DeviceData::poorSignal = 0;
unsigned char DeviceData::attention = 0;
unsigned char DeviceData::meditation = 0;

int DeviceData::EEGData::delta = 0;
int DeviceData::EEGData::theta = 0;
int DeviceData::EEGData::lowAlpha = 0;
int DeviceData::EEGData::highAlpha = 0;
int DeviceData::EEGData::lowBeta = 0;
int DeviceData::EEGData::highBeta = 0;
int DeviceData::EEGData::lowGamma = 0;
int DeviceData::EEGData::midGamma = 0;

void DeviceData::reset(){
	// Icky looking piece of duplication this.
	DeviceData::update = false;
	DeviceData::connecting = false;
	DeviceData::portName = "none";
	
	DeviceData::poorSignal = 0;
	DeviceData::attention = 0;
	DeviceData::meditation = 0;

	DeviceData::EEGData::delta = 0;
	DeviceData::EEGData::theta = 0;
	DeviceData::EEGData::lowAlpha = 0;
	DeviceData::EEGData::highAlpha = 0;
	DeviceData::EEGData::lowBeta = 0;
	DeviceData::EEGData::highBeta = 0;
	DeviceData::EEGData::lowGamma = 0;
	DeviceData::EEGData::midGamma = 0;
};

/*
Initialised inline. See header.  
DeviceDataThread::DeviceDataThread(NanCallback* callback, Mindwave* mindwaveDevice){};
*/

DeviceDataThread::~DeviceDataThread(){};

void DeviceDataThread::Execute(){
	readPorts?
		localDeviceReference -> readPorts(localDeviceReference):
		localDeviceReference -> readDevice(localDeviceReference);
}

void DeviceDataThread::HandleOKCallback(){
	NanScope();
    v8::Local<v8::Value> argv[] = { NanNew<v8::Boolean>(DeviceData::update) };
	callback -> Call(1, argv);
}

// Will use the default NAN HandleErrorCallback for now.