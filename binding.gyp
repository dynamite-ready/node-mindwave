{
	"targets": [
		{
			"target_name": "nodeMindwave",
			"include_dirs": [".", "..", "include", "nan", "thinkgear"],
			"sources": ["node-mindwave.cpp"],
			"conditions": [
				[
					"OS == 'win'", {
						"libraries": ["-lwinmm", "-mwindows", "-lsetupapi"],
						"sources": [
							"src/serial.cc",
							"src/impl/win.cc",
							"src/impl/list_ports/list_ports_win.cc",
							"thinkgear/ThinkGearStreamParser.c",
							"device-data-thread.cpp",
							"device.cpp"
						] 
					 }
				]		
			]
		}
	]
}
