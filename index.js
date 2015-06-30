var mindwaveDevice = require("./build/Release/nodeMindwave");
var e = require("events");
var context = global;

var connectionPollInterval = 200;
var dataPollInterval = 1000;
var hotplugFailsafe = 60; // 12 seconds ((ms / ticks) * seconds);
var hotplugFailsafeCount = 0;

function Device(){
	var _self = this;
	_self.device = new mindwaveDevice.Mindwave();
	_self.events = new e.EventEmitter();
	_self.device.open(function(status){
		function triggerError(){
			hotplugFailsafeCount = 0;
			context.clearInterval(_self.pollForConnection);
			if(_self.events) _self.events.emit("restart");
		}
		
		_self.device.poll(function(){
			// If this has called back, then the infinite thread has stopped.
			// Kill the app.
			triggerError();
		})
		
		if(status){
			_self.pollForConnection = context.setInterval(function(){
				hotplugFailsafeCount++;
				if(hotplugFailsafeCount >= hotplugFailsafe){
					triggerError();
					return false;					
				} else if(!_self.device.running){
					triggerError();
					return false;
				} else if(_self.device.running && !_self.device.connecting){
					hotplugFailsafeCount = 0;
					context.clearInterval(_self.pollForConnection);
					run();
					
					return false;
				} else {
					if(_self.events) _self.events.emit("connecting", { 
						running: _self.device.running, connecting: _self.device.connecting 
					});
				}
			}, connectionPollInterval);
		} else triggerError();
	});
	
	function run(){
		_self.poll = context.setInterval(function(){
			if(!_self.device.running){
				context.clearInterval(_self.poll);
				if(_self.events) _self.events.emit("restart");
				return false;
			} else {
				if(_self.events) _self.events.emit("data", 
					{
						data: _self.device.data, 
						meta: { 
							running: _self.device.running, 
							connecting: _self.device.connecting 
						}
					}
				);
			}
		}, dataPollInterval);			
	}
}

Device.prototype = {
	close: function(callback){
		var _self = this;
		
		if(_self.pollForConnection) context.clearInterval(_self.pollForConnection);
		if(_self.poll) context.clearInterval(_self.poll);
		_self.device.close();
		// delete this.events;
		
		if(callback) callback();
	}
};

process.on("uncaughtException", 
	function(e){ 
		console.log(e); 
		console.log(e.stack); 
	}
);

module.exports = Device;