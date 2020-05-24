#pragma once

#include "secrets.h" 
#include "Thing.h"
#include "WebThingAdapter.h"

class RemoteConfigurationController {
	
	public:
		RemoteConfigurationController( uint8_t chipSelectPin, uint8_t irqPin, uint8_t resetPin, uint8_t enablePin );  // Pins for Adafruit Feather M0 with ATWINC1500
		void setup();
		void update();
		void shutdown();
		bool inContinousMode();
		uint16_t getDuration();
		uint16_t getTotalFrames();
		uint16_t getFrameInterval();
		uint16_t getTriggerPressTime();
		bool isStarted();
		
	private:
		uint8_t _chipSelectPin; 
		uint8_t _irqPin;
		uint8_t _resetPin;
		uint8_t _enablePin;
		
		WebThingAdapter* _adapter;
		ThingDevice* _yamts;
		ThingProperty* _totalFramesProperty;
		ThingProperty* _continousModeProperty;
		ThingProperty* _durationProperty;
		ThingProperty* _frameIntervalProperty;
		ThingProperty* _triggerPressTimeProperty;
		ThingProperty* _startedProperty;
	
};
