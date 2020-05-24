#include <Arduino.h>
#include "MotionController.h"

MotionController::MotionController(uint8_t headSensorPin, uint8_t tailSensorPin, uint8_t stepPin, uint8_t directionPin)
	: _headSensorPin(headSensorPin), _tailSensorPin(tailSensorPin), _stepPin(stepPin), _directionPin(directionPin)
{

	pinMode(_stepPin, OUTPUT);
	pinMode(_directionPin, OUTPUT);
	digitalWrite(_directionPin, HIGH);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	pinMode(_headSensorPin, INPUT_PULLUP);
	pinMode(_tailSensorPin, INPUT_PULLUP);
}

uint16_t MotionController::getHeadPosition()
{
	return MAX_STEP_COUNT;
}

uint16_t MotionController::getTailPosition()
{
	return 0;
}

uint16_t MotionController::getCurrentPosition()
{
	return _currentPosition;
}

bool MotionController::atHead()
{
	return digitalRead(_headSensorPin) == LOW;
}

bool MotionController::atTail()
{
	return digitalRead(_tailSensorPin) == LOW;
}

bool MotionController::movingToHead()
{
	return _state != STOPPED && digitalRead(_directionPin) == LOW;
}

bool MotionController::directionToHead()
{
	return digitalRead(_directionPin) == LOW;
}

bool MotionController::movingToTail()
{
	return _state != STOPPED && digitalRead(_directionPin) == HIGH;
}

bool MotionController::directionToTail()
{
	return digitalRead(_directionPin) == HIGH;
}

void MotionController::goToPosition(uint16_t targetPosition)
{
	if (isStopped())
	{ // Change the target position only when not moving
		_targetPosition = targetPosition;
		setContinousMode(false);
	}
}

void MotionController::goToPosition(uint16_t targetPosition, uint16_t duration)
{
	if (isStopped())
	{ // Change the target position only when not moving
		_targetPosition = targetPosition;
		_stepInterval = calculateContinousMotionStepInterval(abs(_targetPosition - _currentPosition), duration);
		setContinousMode(true);
	}
}

bool MotionController::moveBy(int16_t delta)
{
	if (delta < 0 && -delta > _targetPosition)
		return false;
	if (delta > 0 && _targetPosition + delta > getHeadPosition())
		return false;
	goToPosition(_targetPosition + delta);
	return true;
}

void MotionController::goToTail()
{
	goToPosition(getTailPosition());
}

void MotionController::goToTail(uint16_t duration)
{
	goToPosition(getTailPosition(), duration);
}

void MotionController::goToHead()
{
	goToPosition(getHeadPosition());
}

void MotionController::goToHead(uint16_t duration)
{
	goToPosition(getHeadPosition(), duration);
}

bool MotionController::isMoving()
{
	return _state != STOPPED;
}

bool MotionController::isStopped()
{
	return _state == STOPPED;
}

bool MotionController::inContinousMode()
{
	return _inContinousMode;
}

void MotionController::setContinousMode(bool on)
{
	if (isStopped())
	{ // Change the mode only when not moving
		_inContinousMode = on;
	}
}

void MotionController::setState(MotionController::State state)
{
	_state = state;
	switch (_state)
	{
	case MOVING:
		_currentStep = 0;
		Serial.print("Moving from: ");
		Serial.println(_currentPosition);
		break;
	case STOPPED:
		_currentStep = 0;
		if (atHead() && directionToHead())
		{
			_currentPosition = getHeadPosition();
		}
		else if (atTail() && directionToTail())
		{
			_currentPosition = getTailPosition();
		}
		Serial.print("Stopped at: ");
		Serial.println(_currentPosition);
		break;
	}
}

void MotionController::step()
{
	if (_inContinousMode)
	{
		stepContinous();
	}
	else
	{
		stepDiscrete();
	}
}

void MotionController::stepDiscrete()
{
	uint16_t thresholdSize = _stepIntervalLookupLength / 2;
	uint16_t upperThreshold = _numSteps - thresholdSize;

	switch (_state)
	{
	case MOVING:
		if ((atHead() && movingToHead()) || (atTail() && movingToTail()))
		{
			if (_numSteps > _stepIntervalLookupLength)
			{
				if (_currentStep < thresholdSize)
				{
					_currentStep = _stepIntervalLookupLength - _currentStep;
				}
				else if (_currentStep < upperThreshold)
				{
					_currentStep = upperThreshold;
				}
			}
			else if (_currentStep < _numSteps / 2)
			{
				_currentStep = _numSteps - _currentStep;
			}
		}

		if (_numSteps > _stepIntervalLookupLength)
		{

			if (_currentStep > upperThreshold)
			{
				_stepInterval = _stepIntervalLookup[thresholdSize + _currentStep - upperThreshold];
			}
			else if (_currentStep < thresholdSize)
			{
				_stepInterval = _stepIntervalLookup[_currentStep];
			}
			else
			{
				_stepInterval = _stepIntervalLookup[thresholdSize]; // middle value
			}
		}
		else
		{
			_stepInterval = _stepIntervalLookup[_currentStep];
		}
		_currentStep++;

		if (_currentStep == _numSteps)
		{
			setState(STOPPED);
		}
		break;
	case STOPPED:
		if (_currentPosition == UNKNOWN_POSITION)
		{
			if (atHead())
			{
				_currentPosition = getHeadPosition();
			}
			else if (atTail())
			{
				_currentPosition = getTailPosition();
			}
			else
			{
				if (_targetPosition > getTailPosition())
				{
					digitalWrite(_directionPin, LOW);
				}
				else
				{
					digitalWrite(_directionPin, HIGH);
				}
				calculateMotorStepIntervalLookup();
				setState(MOVING);
			}
		}
		else
		{
			if ((_targetPosition > _currentPosition) && !atHead())
			{
				digitalWrite(_directionPin, LOW);
				calculateMotorStepIntervalLookup();
				setState(MOVING);
			}
			else if ((_targetPosition < _currentPosition) && !atTail())
			{
				digitalWrite(_directionPin, HIGH);
				calculateMotorStepIntervalLookup();
				setState(MOVING);
			}
		}
		break;
	}

	if (_state != STOPPED)
	{
		digitalWrite(_stepPin, HIGH);
		digitalWrite(_stepPin, LOW);
		if (_currentPosition != UNKNOWN_POSITION)
		{ // Current position must be initialized to be tracked
			_currentPosition += directionToHead() ? 1 : -1;
		}
		delayMicroseconds(_stepInterval);
	}
	else
	{
		delay(100);
	}
}

void MotionController::stepContinous()
{
	if (atHead() && movingToHead())
	{
		Serial.println("Head!");
		setState(STOPPED);
	}
	if (atTail() && movingToTail())
	{
		Serial.println("Tail!");
		setState(STOPPED);
	}

	switch (_state)
	{
	case MOVING:
		if (_currentPosition != UNKNOWN_POSITION)
		{
			if (movingToHead())
			{
				if (_currentPosition >= _targetPosition)
				{
					setState(STOPPED);
				}
			}
			else
			{ // Moving to tail
				if (_currentPosition <= _targetPosition)
				{
					setState(STOPPED);
				}
			}
		}
		break;
	case STOPPED:
		if (_currentPosition == UNKNOWN_POSITION)
		{
			if (atHead())
			{
				_currentPosition = getHeadPosition();
			}
			else if (atTail())
			{
				_currentPosition = getTailPosition();
			}
			else
			{
				if (_targetPosition > getTailPosition())
				{
					digitalWrite(_directionPin, LOW);
				}
				else
				{
					digitalWrite(_directionPin, HIGH);
				}
				setState(MOVING);
			}
		}
		else
		{
			if ((_targetPosition > _currentPosition) && !atHead())
			{
				digitalWrite(_directionPin, LOW);
				setState(MOVING);
			}
			else if ((_targetPosition < _currentPosition) && !atTail())
			{
				digitalWrite(_directionPin, HIGH);
				setState(MOVING);
			}
		}
		break;
	}

	if (_state != STOPPED)
	{
		digitalWrite(_stepPin, HIGH);
		digitalWrite(_stepPin, LOW);
		if (_currentPosition != UNKNOWN_POSITION)
		{ // Current position must be initialized to be tracked
			_currentPosition += directionToHead() ? 1 : -1;
		}
		delayMicroseconds(_stepInterval);
	}
}

void MotionController::calculateMotorStepIntervalLookup()
{
	float x, a;
	uint16_t absDelta = abs(_targetPosition - _currentPosition);
	uint16_t minStepInterval;
	float scalingFactor = absDelta < STEP_INTERVAL_LOOKUP_CAPACITY ? 1.0 - 1.0 * absDelta / STEP_INTERVAL_LOOKUP_CAPACITY : 0.0;

	_numSteps = absDelta;
	_stepIntervalLookupLength = min(absDelta, STEP_INTERVAL_LOOKUP_CAPACITY);
	minStepInterval = MIN_STEP_INTERVAL + scalingFactor * STEP_INTERVAL_SPAN; // this value will be rounded later

	Serial.print("_numSteps: ");
	Serial.println(_numSteps);
	Serial.print("_stepIntervalLookupLength: ");
	Serial.println(_stepIntervalLookupLength);
	Serial.print("scalingFactor: ");
	Serial.println(scalingFactor);
	Serial.print("minStepInterval: ");
	Serial.println(minStepInterval);

	for (uint16_t i = 0; i < _stepIntervalLookupLength; i++)
	{
		x = i / (_stepIntervalLookupLength - 1.0);
		//a = 16.0 * (x - 0.5) * (x - 0.5) * (x - 0.5) * (x - 0.5);
		a = 4.0 * (x - 0.5) * (x - 0.5);
		_stepIntervalLookup[i] = round(minStepInterval + a * (MAX_STEP_INTERVAL - minStepInterval));
	}
}

uint16_t MotionController::calculateContinousMotionStepInterval(uint16_t distance, uint16_t duration)
{
	uint64_t durationMicros = duration * 1000000; // from seconds to microseconds
	return durationMicros / distance;
}
