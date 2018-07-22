#pragma once

#include "GallantSignal.h"
using Gallant::Signal0;

class EnvelopeGenerator {
public:
	enum Stage {
		ENVELOPE_STAGE_OFF = 0,
		ENVELOPE_STAGE_ATTACK,
		ENVELOPE_STAGE_DECAY,
		ENVELOPE_STAGE_SUSTAIN,
		ENVELOPE_STAGE_RELEASE,
		kNumStages
	};

	const double minimumLevel;

	EnvelopeGenerator();

	Signal0<> beganEnvelopeCycle;
	Signal0<> finishedEnvelopeCycle;

	void enterStage(Stage stage);
	double nextSample();
	static void setSampleRate(double sampleRate);
	void setStageValue(Stage stage, double value);
	void reset();
	
	inline Stage getCurrentStage() const {
		return currentStage_;
	}

private:
	Stage currentStage_;
	double currentLevel_;
	double multiplier_;
	static double sampleRate_;
	double stageValue_[kNumStages];
	size_t currentSampleIndex_;
	size_t nextStageSampleIndex_;

	void calculateMultiplier(double startLevel, double endLevel, size_t lengthInSamples);
};
