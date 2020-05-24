/* 
 * Tested with stepper motor driver at 14V
 * Number of impulses needed to travel the whole availabale distance in 1/16-step mode: ~44848 (2803 whole steps)
 * 
 * Power consumption:
 * - Total:                         350mAh
 * - Behind voltage regulator:       18mAh 
 * 
 * Motor Pins:
 * - R -> 2B
 * - B -> 2A
 * - G -> 1A
 * - Y -> 1B
 * 
 */
 
 
#include "MotionController.h"
#include "CameraTriggerController.h"
#include "RemoteConfigurationController.h"

const uint8_t PIN_HEAD_LIMIT_SENSOR = 10;
const uint8_t PIN_TAIL_LIMIT_SENSOR = 5;  // Can't use pin 9 because it is used for battery voltage measurements
const uint8_t PIN_STEP = A0;
const uint8_t PIN_DIRECTION = A1;
const uint8_t PIN_CAMERA_TRIGGER = 11;
const uint8_t PIN_POWER_LED = 6;
const uint8_t PIN_CHIP_SELECT = 8;
const uint8_t PIN_IRQ = 7;
const uint8_t PIN_RESET = 4;
const uint8_t PIN_ENABLE = 2;

const uint8_t MIN_TRIGGER_PRESS_TIME_MILLIS = 10;

const uint16_t CONFIGURATION_MODE_BLINK_INTERVAL = 200; // [ms]
const uint16_t RUNNING_MODE_BLINK_DURATION = 50;  			// [ms]

CameraTriggerController cameraTriggerController(PIN_CAMERA_TRIGGER);
MotionController motionController( PIN_HEAD_LIMIT_SENSOR, PIN_TAIL_LIMIT_SENSOR, PIN_STEP, PIN_DIRECTION );
RemoteConfigurationController remoteConfigurationController(PIN_CHIP_SELECT, PIN_IRQ, PIN_RESET, PIN_ENABLE);

bool running = false;
bool returningToCentre = false;
bool inConfiguration = true; 

// User settings (with initial values):
uint16_t triggerPressTime = 100; 				// [ms] = 0.1 seconds
uint16_t totalFrames = 120;
uint16_t frameInterval = 4000; 					// [ms] = 4 seconds
uint16_t continousMotionDuration = 60; 	// [s] = 1 minute
bool inContinousMode = false;

// Calculated settings:
uint16_t stepsBetweenFrames;
uint16_t delayBeforeShot = 100;  	// [ms]
int32_t frameStartTime = -1; 			// [ms]

// Other variables:
uint16_t configurationModeBlinkStart = 0;


void setup() {    
	pinMode(PIN_POWER_LED, OUTPUT);
  switchPowerLedOn();

  Serial.begin(9600);
	//while (!Serial);
  Serial.println("Starting...");
  
	remoteConfigurationController.setup();    
}

void loop() {	
	if (inConfiguration) {
		configurationUpdate();
	} else if (running || returningToCentre) {
		motionUpdate();
	} else {		
		// Do nothing
		delay(1000);
	}
}

void togglePowerLed() {
	digitalWrite(PIN_POWER_LED, !digitalRead(PIN_POWER_LED));
}

void switchPowerLedOn() {
	digitalWrite(PIN_POWER_LED, HIGH);
}

void switchPowerLedOff() {
	digitalWrite(PIN_POWER_LED, LOW);
}

void blinkPowerLed( uint8_t millis ) {
	digitalWrite(PIN_POWER_LED, LOW);
	digitalWrite(PIN_POWER_LED, HIGH);
	delay(millis);
	digitalWrite(PIN_POWER_LED, LOW);
}

void configurationUpdate() {
  remoteConfigurationController.update();
	if ( remoteConfigurationController.isStarted() ) {
 		inContinousMode = remoteConfigurationController.inContinousMode();
 		continousMotionDuration = remoteConfigurationController.getDuration();
 		totalFrames = remoteConfigurationController.getTotalFrames();
		frameInterval = remoteConfigurationController.getFrameInterval() * 1000;  // [ms]
		triggerPressTime = max( MIN_TRIGGER_PRESS_TIME_MILLIS, remoteConfigurationController.getTriggerPressTime() * 1000 );   // [ms]		
		stepsBetweenFrames = motionController.getHeadPosition()/totalFrames;
				
		/*
		Serial.print("totalFrames = " );
	  Serial.println(totalFrames);
		Serial.print("frameInterval = ");
		Serial.println(frameInterval);
		Serial.print("triggerPressTime = ");
		Serial.println(triggerPressTime);
		*/
		
		remoteConfigurationController.shutdown();
	  motionController.goToTail();
	  inConfiguration = false;
	  running = true;
	}
	if ( millis() - configurationModeBlinkStart > CONFIGURATION_MODE_BLINK_INTERVAL ) {
		togglePowerLed();
		configurationModeBlinkStart = millis();
	}
}

void motionUpdate() {
	bool canContinue = true;	
	uint16_t delayAfterShot;

  motionController.step();
  if ( motionController.isStopped() ) {
  	if (running) {
  		if ( inContinousMode ) {
  			motionController.goToHead(continousMotionDuration);  // [s]
  		} else {
				delay( delayBeforeShot );  // Wait for the camera to stabilize after stopping
				blinkPowerLed(RUNNING_MODE_BLINK_DURATION);
				cameraTriggerController.shoot( triggerPressTime );
				if ( frameStartTime != -1 ) {
					delayAfterShot = frameInterval - (millis() - frameStartTime);
				} else {
					delayAfterShot = frameInterval - (delayBeforeShot+triggerPressTime);
				}
				delay( delayAfterShot );
				frameStartTime = millis();
				canContinue = motionController.moveBy(stepsBetweenFrames);
			}
		}
		if (returningToCentre && motionController.isStopped() ) {
			// Returned to centre position. Set all mode flags to "false":
			returningToCentre = false;
			inConfiguration = false;
		  running = false;
		  switchPowerLedOn();
		} else if (motionController.atHead() || !canContinue) {
			motionController.setContinousMode(false);
			motionController.goToPosition( motionController.getHeadPosition()/2 );
			running = false;
			returningToCentre = true;
		}
  }
}
