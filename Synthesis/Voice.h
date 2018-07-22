#pragma once

#include "Oscillator.h"
#include "EnvelopeGenerator.h"
#include "Filter.h"

class Voice {
public:
	Voice();

	double nextSample();
	void setFree();
	void reset();

	inline bool isActive() const {
		return isActive_;
	}

	inline int noteNumber() const {
		return noteNumber_;
	}

	inline Oscillator& oscillator1() {
		return oscillator1_;
	}

	inline Oscillator& oscillator2() {
		return oscillator2_;
	}

	inline EnvelopeGenerator& volumeEnvelope() {
		return volumeEnvelope_;
	}

	inline EnvelopeGenerator& filterEnvelope() {
		return filterEnvelope_;
	}

	inline Filter& filter() {
		return filter_;
	}

	inline void setIsActive(bool isActive) {
		isActive_ = isActive;
	}

	inline void setVelocity(int velocity) {
		velocity_ = velocity;
	}

	inline void setFilterEnvelopeAmount(double amount) {
		filterEnvelopeAmount_ = amount;
	}

	inline void setFilterLfoAmount(double amount) {
		filterLfoAmount_ = amount;
	}

	inline void setOscillator1PitchAmount(double amount) {
		oscillator1PitchAmount_ = amount;
	}

	inline void setOscillator2PitchAmount(double amount) {
		oscillator2PitchAmount_ = amount;
	}

	inline void setOscillatorMix(double mix) {
		oscillatorMix_ = mix;
	}

	inline void setLfoValue(double value) {
		lfoValue_ = value;
	}

	inline void setNoteNumber(int noteNumber) {
		noteNumber_ = noteNumber;
		auto frequency = 440.0 * pow(2.0, (noteNumber_ - 69.0) / 12.0);
		oscillator1_.setFrequency(frequency);
		oscillator2_.setFrequency(frequency);
	}

private:
	Oscillator oscillator1_;
	Oscillator oscillator2_;
	EnvelopeGenerator volumeEnvelope_;
	EnvelopeGenerator filterEnvelope_;
	Filter filter_;
	int noteNumber_;
	int velocity_;
	double filterEnvelopeAmount_;
	double oscillatorMix_;
	double filterLfoAmount_;
	double oscillator1PitchAmount_;
	double oscillator2PitchAmount_;
	double lfoValue_;
	bool isActive_;
};