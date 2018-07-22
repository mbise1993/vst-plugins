#include "EnvelopeGenerator.h"
#include <cmath>

double EnvelopeGenerator::sampleRate_ = 44100.0;

EnvelopeGenerator::EnvelopeGenerator() :
	minimumLevel(0.0001),
	currentStage_(ENVELOPE_STAGE_OFF),
	currentLevel_(minimumLevel),
	multiplier_(1.0),
	currentSampleIndex_(0),
	nextStageSampleIndex_(0) {

	stageValue_[ENVELOPE_STAGE_OFF] = 0.0;
	stageValue_[ENVELOPE_STAGE_ATTACK] = 0.01;
	stageValue_[ENVELOPE_STAGE_DECAY] = 0.5;
	stageValue_[ENVELOPE_STAGE_SUSTAIN] = 0.1;
	stageValue_[ENVELOPE_STAGE_RELEASE] = 1.0;
}

double EnvelopeGenerator::nextSample() {
	if (currentStage_ != ENVELOPE_STAGE_OFF && currentStage_ != ENVELOPE_STAGE_SUSTAIN) {
		if (currentSampleIndex_ == nextStageSampleIndex_) {
			auto newStage = static_cast<Stage>((currentStage_ + 1) % kNumStages);
			enterStage(newStage);
		}

		currentLevel_ *= multiplier_;
		currentSampleIndex_++;
	}

	return currentLevel_;
}

void EnvelopeGenerator::enterStage(Stage stage) {
	if (currentStage_ == stage) {
		return;
	}

	if (currentStage_ == ENVELOPE_STAGE_OFF) {
		beganEnvelopeCycle();
	}

	if (stage == ENVELOPE_STAGE_OFF) {
		finishedEnvelopeCycle();
	}

	currentStage_ = stage;
	currentSampleIndex_ = 0;
	if (currentStage_ == ENVELOPE_STAGE_OFF || currentStage_ == ENVELOPE_STAGE_SUSTAIN) {
		nextStageSampleIndex_ = 0;
	} else {
		nextStageSampleIndex_ = stageValue_[currentStage_] * sampleRate_;
	}

	switch (stage) {
		case ENVELOPE_STAGE_OFF:
			currentLevel_ = 0.0;
			multiplier_ = 1.0;
			break;
		case ENVELOPE_STAGE_ATTACK:
			currentLevel_ = minimumLevel;
			calculateMultiplier(currentLevel_, 1.0, nextStageSampleIndex_);
			break;
		case ENVELOPE_STAGE_DECAY:
			currentLevel_ = 1.0;
			calculateMultiplier(currentLevel_, fmax(stageValue_[ENVELOPE_STAGE_SUSTAIN], minimumLevel), nextStageSampleIndex_);
			break;
		case ENVELOPE_STAGE_SUSTAIN:
			currentLevel_ = stageValue_[ENVELOPE_STAGE_SUSTAIN];
			multiplier_ = 1.0;
			break;
		case ENVELOPE_STAGE_RELEASE:
			calculateMultiplier(currentLevel_, minimumLevel, nextStageSampleIndex_);
			break;
		default:
			break;
	}
}

void EnvelopeGenerator::setStageValue(Stage stage, double value) {
	stageValue_[stage] = value;
	if (stage == currentStage_) {
		// Recalculate multiplier and nextStageSampleIndex
		if (currentStage_ == ENVELOPE_STAGE_ATTACK ||
			currentStage_ == ENVELOPE_STAGE_DECAY ||
			currentStage_ == ENVELOPE_STAGE_RELEASE) {
			double nextLevelValue;
			switch (currentStage_) {
				case ENVELOPE_STAGE_ATTACK:
					nextLevelValue = 1.0;
					break;
				case ENVELOPE_STAGE_DECAY:
					nextLevelValue = fmax(stageValue_[ENVELOPE_STAGE_SUSTAIN], minimumLevel);
					break;
				case ENVELOPE_STAGE_RELEASE:
					nextLevelValue = minimumLevel;
					break;
				default:
					break;
			}

			// How far the generator is into the current stage
			auto currentStageProcess = (currentSampleIndex_ + 0.0) / nextStageSampleIndex_;
			// How much of the current stage is left
			auto remainingStageProcess = 1.0 - currentStageProcess;
			auto samplesUntilNextStage = remainingStageProcess * value * sampleRate_;
			nextStageSampleIndex_ = currentSampleIndex_ + samplesUntilNextStage;
			calculateMultiplier(currentLevel_, nextLevelValue, samplesUntilNextStage);
		} else if (currentStage_ == ENVELOPE_STAGE_SUSTAIN) {
			currentLevel_ = value;
		}
	}

	if (currentStage_ == ENVELOPE_STAGE_DECAY && stage == ENVELOPE_STAGE_SUSTAIN) {
		// Decay different sustain value than before. Recalc multiplier
		auto samplesUntilNextStage = nextStageSampleIndex_ - currentSampleIndex_;
		calculateMultiplier(
			currentLevel_,
			fmax(stageValue_[ENVELOPE_STAGE_SUSTAIN], minimumLevel),
			samplesUntilNextStage);
	}
}

void EnvelopeGenerator::setSampleRate(double sampleRate) {
	sampleRate_ = sampleRate;
}

void EnvelopeGenerator::calculateMultiplier(double startLevel, double endLevel, size_t lengthInSamples) {
	multiplier_ = 1.0 + (log(endLevel) - log(startLevel)) / lengthInSamples;
}

void EnvelopeGenerator::reset() {
	currentStage_ = ENVELOPE_STAGE_OFF;
	currentLevel_ = minimumLevel;
	multiplier_ = 1.0;
	currentSampleIndex_ = 0;
	nextStageSampleIndex_ = 0;
}
