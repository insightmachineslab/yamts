#include <Arduino.h>
#include "CameraTriggerController.h"

CameraTriggerController::CameraTriggerController( uint8_t triggerPin )
  : _triggerPin(triggerPin) {
  
  pinMode(_triggerPin, OUTPUT);
  digitalWrite(_triggerPin, LOW);
}

void CameraTriggerController::shoot( uint16_t durationMs ) {
	if ( durationMs < MINIMUM_SHUTTER_PRESS_DURATION ) durationMs = MINIMUM_SHUTTER_PRESS_DURATION;
	digitalWrite(_triggerPin, HIGH);  
  delay(durationMs);
  digitalWrite(_triggerPin, LOW);    
}
