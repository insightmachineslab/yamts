#pragma once
class CameraTriggerController {
	
	public:
		CameraTriggerController( uint8_t triggerPin );
		void shoot( uint16_t durationMs );
		
	private:
   	const static uint16_t MINIMUM_SHUTTER_PRESS_DURATION = 10;  // [ms]
		uint8_t _triggerPin = 4;
	
};
