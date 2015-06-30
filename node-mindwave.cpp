#include <nan.h>
#include <device.hpp>
#include <device-data-thread.hpp>

using namespace v8;
using namespace node;

void DeviceRegister(Handle<Function> target) {
    Device::initialise(target);
}

NODE_MODULE(nodeMindwave, DeviceRegister)