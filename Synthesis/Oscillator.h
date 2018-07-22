#pragma once

class Oscillator {
public:
	enum Mode {
		OSCILLATOR_MODE_SINE = 0,
		OSCILLATOR_MODE_SAW,
		OSCILLATOR_MODE_SQUARE,
		OSCILLATOR_MODE_TRIANGLE,
		kNumOscillatorModes
	};

	Oscillator();

	void setMode(Mode mode);
	void setFrequency(double frequency);
	void setSampleRate(double sampleRate);
	void setPitchMod(double amount);
	double nextSample();
	void reset();

private:
	Mode mode_;
	const double pi_;
	const double twoPi_;
	double frequency_;
	double phase_;
	static double sampleRate_;
	double phaseIncrement_;
	double pitchMod_;

	void updateIncrement();
};
