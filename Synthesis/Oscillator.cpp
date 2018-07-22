#include "Oscillator.h"
#include <math.h>

double Oscillator::sampleRate_ = 44100.0;

Oscillator::Oscillator() :
	mode_(OSCILLATOR_MODE_SINE),
	pi_(2 * acos(0.0)),
	twoPi_(2.0 * pi_),
	frequency_(440.0),
	phase_(0.0),
	pitchMod_(0.0) {

	updateIncrement();
}

void Oscillator::setMode(Mode mode) {
	mode_ = mode;
}

void Oscillator::setFrequency(double frequency) {
	frequency_ = frequency;
	updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate) {
	sampleRate_ = sampleRate;
	updateIncrement();
}

void Oscillator::setPitchMod(double amount) {
	pitchMod_ = amount;
	updateIncrement();
}

void Oscillator::updateIncrement() {
	auto pitchModAsFrequency = pow(2.0, fabs(pitchMod_) * 14.0) - 1;
	if (pitchMod_ < 0) {
		pitchModAsFrequency = -pitchModAsFrequency;
	}

	auto calculatedFrequency = fmin(fmax(frequency_ + pitchModAsFrequency, 0), sampleRate_ / 2.0);
	phaseIncrement_ = calculatedFrequency * 2.0 * pi_ / sampleRate_;
}

double Oscillator::nextSample() {
	auto value = 0.0;

	switch (mode_) {
		case OSCILLATOR_MODE_SINE:
			value = sin(phase_);
			break;
		case OSCILLATOR_MODE_SAW:
			value = 1.0 - (2.0 * phase_ / twoPi_);
			break;
		case OSCILLATOR_MODE_SQUARE:
			value = phase_ <= pi_ ? 1.0 : -1.0;
			break;
		case OSCILLATOR_MODE_TRIANGLE:
			value = -1.0 + (2.0 * phase_ / twoPi_);
			value = 2.0 * (fabs(value) - 0.5);
			break;
	}

	phase_ += phaseIncrement_;
	while (phase_ >= twoPi_) {
		phase_ -= twoPi_;
	}

	return value;
}

void Oscillator::reset() {
	phase_ = 0.0;
}
