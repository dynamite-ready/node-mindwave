#Node Mindwave v0.1.0

A Node JS cross platform native module for the Neurosky Mindwave - http://store.neurosky.com/products/mindwave-1.
Current only works on Windows. I have plans to add Mac support. Linux support will depend on time and demand. Basic instructions for use are as follows:

###Installation

###Prerequisites for Windows:

You will have ideally installed the Neurosky Mindwave driver using the CD that came with the peripheral,
if not, it's a component of Neurosky's "Mindwave Bundle for Windows".
You can find the latest version of the package here - http://download.neurosky.com/updates/mindwave/education/1.1.28.0/MindWave.zip

###All systems:

Make sure you have Node JS installed, obviously. Node-Gyp is essential too.
Copy the repo to your machine. Using the command terminal, 'cd' into the repo/module and run:

```
node-gyp configure
node-gyp build
```

After doing so, you should be able to use the methods found in index.js.
NPM support coming soon.

###Basic Usage:

Given the nature of the Mindwave's operation it's API is a little unusual. However, it's event based and fairly simple.
You're essentially required to create a new instance of the device, and then use the events it triggers to poll the device.

The example below demonstrates all the available events.
You can also "close" the device at anytime with the "mindwave.close()" method.
The close method will accept a callback, so you can remove all errant event handlers before shutting down.

```javascript
var mindwave = new Device();

mindwave.events.on("connecting", function(status){ console.log(status); });
mindwave.events.on("data", function(data){ console.log(data); });
mindwave.events.on("restart", function(){ console.log("Device Not Found: Restart"); });

```
