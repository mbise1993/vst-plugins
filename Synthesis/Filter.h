#pragma once

#include <cmath>

class Filter {
public:
	enum Mode {
		FILTER_MODE_LOWPASS = 0,
		FILTER_MODE_HIGHPASS,
		FILTER_MODE_BANDPASS,
		kNumFilterModes
	};

	Filter();

	double process(double inputValue);
	void reset();

	inline void setCutoff(double cutoff) {
		cutoff_ = cutoff;
		calculateFeedbackAmount();
	}

	inline void setCutoffMod(double cutoffMod) {
		cutoffMod_ = cutoffMod;
		calculateFeedbackAmount();
	}

	inline void setResonance(double resonance) {
		resonance_ = resonance;
		calculateFeedbackAmount();
	}

	inline void setFilterMode(Mode mode) {
		mode_ = mode;
	}

private:
	double cutoff_;
	double cutoffMod_;
	double resonance_;
	Mode mode_;
	double feedbackAmount_;
	double buf0;
	double buf1;
	double buf2;
	double buf3;

	inline void calculateFeedbackAmount() {
		feedbackAmount_ = resonance_ + resonance_ / (1.0 - getCalculatedCutoff());
	}

	inline double getCalculatedCutoff() const {
		return fmax(fmin(cutoff_ + cutoffMod_, 0.99), 0.01);
	}
};
