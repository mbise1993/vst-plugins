#include "VoiceManager.h"

void VoiceManager::onNoteOn(int noteNumber, int velocity) {
	auto voice = findFreeVoice();
	if (!voice) {
		return;
	}

	voice->reset();
	voice->setNoteNumber(noteNumber);
	voice->setVelocity(velocity);
	voice->setIsActive(true);
	voice->volumeEnvelope().enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
	voice->filterEnvelope().enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
}

void VoiceManager::onNoteOff(int noteNumber, int velocity) {
	for (auto i = 0; i < NumberOfVoices; ++i) {
		auto& voice = voices_[i];
		if (voice.isActive() && voice.noteNumber() == noteNumber) {
			voice.volumeEnvelope().enterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
			voice.filterEnvelope().enterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
		}
	}
}

double VoiceManager::nextSample() {
	auto output = 0.0;
	auto lfoValue = lfo_.nextSample();
	for (auto i = 0; i < NumberOfVoices; ++i) {
		auto& voice = voices_[i];
		voice.setLfoValue(lfoValue);
		output += voice.nextSample();
	}

	return output;
}

Voice* VoiceManager::findFreeVoice() {
	Voice* freeVoice = nullptr;
	for (auto i = 0; i < NumberOfVoices; ++i) {
		if (!voices_[i].isActive()) {
			freeVoice = &(voices_[i]);
			break;
		}
	}

	return freeVoice;
}

void VoiceManager::setSampleRate(double sampleRate) {
	EnvelopeGenerator::setSampleRate(sampleRate);
	for (auto i = 0; i < NumberOfVoices; ++i) {
		auto& voice = voices_[i];
		voice.oscillator1().setSampleRate(sampleRate);
		voice.oscillator2().setSampleRate(sampleRate);
	}

	lfo_.setSampleRate(sampleRate);
}