#include <Arduino.h>
#include "RemoteConfigurationController.h"

RemoteConfigurationController::RemoteConfigurationController( uint8_t chipSelectPin, uint8_t irqPin, uint8_t resetPin, uint8_t enablePin )
	: _chipSelectPin(chipSelectPin), _irqPin(irqPin), _resetPin(resetPin), _enablePin(enablePin) {

	const char* deviceTypes[] = {"Thing", nullptr};
	
	_continousModeProperty = new ThingProperty("continousMode", "Set to true to run in continous mode", BOOLEAN, nullptr);
	_durationProperty = new ThingProperty("duration", "Duration of the continous movement in seconds", NUMBER, nullptr);
	_totalFramesProperty = new ThingProperty("totalFrames", "Number of total frames", NUMBER, nullptr);
	_frameIntervalProperty = new ThingProperty("frameInterval", "Frame interval in seconds", NUMBER, nullptr);
	_triggerPressTimeProperty = new ThingProperty("triggerPressTime", "Camera trigger press time in milliseconds", NUMBER, nullptr);
	_startedProperty = new ThingProperty("started", "Set to true to start the slider", BOOLEAN, nullptr);
	
	_yamts = new ThingDevice("yamts", "Yet Another Motorized Timelapse Slider", deviceTypes);
	_yamts->addProperty(_continousModeProperty);
	_yamts->addProperty(_durationProperty);
	_yamts->addProperty(_totalFramesProperty);
  _yamts->addProperty(_frameIntervalProperty);
  _yamts->addProperty(_triggerPressTimeProperty);
  _yamts->addProperty(_startedProperty);
}

void RemoteConfigurationController::setup() {
	Serial.print("Creating access point named: ");
  Serial.println(SSID);
  
  WiFi.setPins(_chipSelectPin, _irqPin, _resetPin, _enablePin); 
  int status = WiFi.beginAP(SSID, 0, PASSWORD);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait for connection:
  delay(5000);

  IPAddress ip = WiFi.localIP();

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(ip);
  
  _adapter = new WebThingAdapter("w25", ip);
  _adapter->addDevice(_yamts);
  _adapter->begin();
  
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(ip);
  Serial.print("/things/");
  Serial.println(_yamts->id);
  
}

void RemoteConfigurationController::update() {
	_adapter->update();
}

void RemoteConfigurationController::shutdown() {
	WiFi.disconnect(); 
}

bool RemoteConfigurationController::inContinousMode() {
	return _continousModeProperty->getValue().boolean;
}

uint16_t RemoteConfigurationController::getDuration() {
	return _durationProperty->getValue().number;
}

uint16_t RemoteConfigurationController::getTotalFrames() {
	return _totalFramesProperty->getValue().number;
}

uint16_t RemoteConfigurationController::getFrameInterval() {
	return _frameIntervalProperty->getValue().number;
} 

uint16_t RemoteConfigurationController::getTriggerPressTime() {
	return _triggerPressTimeProperty->getValue().number;
}

bool RemoteConfigurationController::isStarted() {
	return _startedProperty->getValue().boolean;
}

