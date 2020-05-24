#pragma once
class MotionController
{

public:
	MotionController(uint8_t headSensorPin, uint8_t tailSensorPin, uint8_t stepPin, uint8_t directionPin);
	uint16_t getHeadPosition();
	uint16_t getTailPosition();
	uint16_t getCurrentPosition();
	void goToPosition(uint16_t pos);
	void goToPosition(uint16_t pos, uint16_t duration);
	void goToTail();
	void goToTail(uint16_t duration); // [s]
	void goToHead();
	void goToHead(uint16_t duration); // [s]
	bool moveBy(int16_t delta);
	bool atHead();
	bool atTail();
	bool movingToHead();
	bool directionToHead();
	bool movingToTail();
	bool directionToTail();
	void step();
	bool isMoving();
	bool isStopped();
	bool inContinousMode();
	void setContinousMode(bool on);

private:
	enum State
	{
		MOVING,
		STOPPED
	};

	const static uint16_t UNKNOWN_POSITION = 65535;
	const static uint16_t MAX_STEP_COUNT = 44848;
	const static uint16_t MIN_INTERRUPT_INTERVAL = 10;		  						// [ms]
	const static uint16_t MIN_STEP_INTERVAL = 400;			  						// [us]
	const static uint16_t MAX_STEP_INTERVAL = 6000;			  						// [us]	
	const static uint16_t STEP_INTERVAL_SPAN = MAX_STEP_INTERVAL-MIN_STEP_INTERVAL; // [us]	
	const static uint16_t STEP_INTERVAL_LOOKUP_CAPACITY = 1000;

	uint16_t _stepInterval = MAX_STEP_INTERVAL;
	uint16_t _stepIntervalLookup[STEP_INTERVAL_LOOKUP_CAPACITY];
	uint16_t _targetPosition = 0;
	uint16_t _currentPosition = UNKNOWN_POSITION;	
	uint16_t _currentStep = 0;
	uint16_t _numSteps = 0;
	uint16_t _stepIntervalLookupLength = 0;
	
	bool _inContinousMode = false;

	uint8_t _headSensorPin = 10;
	uint8_t _tailSensorPin = 9;
	uint8_t _stepPin = A0;
	uint8_t _directionPin = A1;

	State _state = STOPPED;

private:
	void setState(MotionController::State state);
	void calculateMotorStepIntervalLookup();
	void stepContinous();
	void stepDiscrete();
	uint16_t calculateContinousMotionStepInterval(uint16_t distance, uint16_t duration);
};
