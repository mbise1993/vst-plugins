#include "Voice.h"

Voice::Voice() :
	noteNumber_(-1),
	velocity_(0),
	filterEnvelopeAmount_(0.0),
	filterLfoAmount_(0.0),
	oscillator1PitchAmount_(0.0),
	oscillator2PitchAmount_(0.0),
	oscillatorMix_(0.5),
	lfoValue_(0.0),
	isActive_(false) {

	volumeEnvelope_.finishedEnvelopeCycle.Connect(this, &Voice::setFree);
}

double Voice::nextSample() {
	if (!isActive_) {
		return 0.0;
	}

	auto oscillator1Output = oscillator1_.nextSample();
	auto oscillator2Output = oscillator2_.nextSample();
	auto oscillatorSum = ((1 - oscillatorMix_) * oscillator1Output) + (oscillatorMix_ * oscillator2Output);

	auto volumeEnvelopeValue = volumeEnvelope_.nextSample();
	auto filterEnvelopeValue = filterEnvelope_.nextSample();

	filter_.setCutoffMod(filterEnvelopeValue * filterEnvelopeAmount_ + lfoValue_ * filterLfoAmount_);

	oscillator1_.setPitchMod(lfoValue_ * oscillator1PitchAmount_);
	oscillator2_.setPitchMod(lfoValue_ * oscillator2PitchAmount_);

	return filter_.process(oscillatorSum * volumeEnvelopeValue * velocity_ / 127.0);
}

void Voice::setFree() {
	isActive_ = false;
}

void Voice::reset() {
	noteNumber_ = -1;
	velocity_ = 0;
	oscillator1_.reset();
	oscillator2_.reset();
	volumeEnvelope_.reset();
	filterEnvelope_.reset();
}