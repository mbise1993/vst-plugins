#include "Filter.h"

Filter::Filter() :
	cutoff_(0.99),
	cutoffMod_(0.0),
	resonance_(0.0),
	mode_(FILTER_MODE_LOWPASS),
	buf0(0.0),
	buf1(0.0),
	buf2(0.0),
	buf3(0.0) {

	calculateFeedbackAmount();
}

// By Paul Kellett
// http://www.musicdsp.org/showone.php?id=29
double Filter::process(double inputValue) {
	if (inputValue == 0.0) {
		return inputValue;
	}

	auto calculatedCutoff = getCalculatedCutoff();
	buf0 += calculatedCutoff * (inputValue - buf0 + feedbackAmount_ * (buf0 - buf1));
	buf1 += calculatedCutoff * (buf0 - buf1);
	buf2 += calculatedCutoff * (buf1 - buf2);
	buf3 += calculatedCutoff * (buf2 - buf3);

	switch (mode_) {
		case FILTER_MODE_LOWPASS:
			return buf3;
		case FILTER_MODE_HIGHPASS:
			return inputValue - buf3;
		case FILTER_MODE_BANDPASS:
			return buf0 - buf3;
		default:
			return 0.0;
	}
}

void Filter::reset() {
	buf0 = buf1 = buf2 = buf3 = 0.0;
}