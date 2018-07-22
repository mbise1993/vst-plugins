#pragma once

#include "Voice.h"
#include <functional>

class VoiceManager {
public:
	using VoiceChangerFunction = std::function<void(Voice&)>;

	void onNoteOn(int noteNumber, int velocity);
	void onNoteOff(int noteNumber, int velocity);
	double nextSample();
	void setSampleRate(double sampleRate);

	inline void setLfoMode(Oscillator::Mode mode) {
		lfo_.setMode(mode);
	}

	inline void setLfoFrequency(double frequency) {
		lfo_.setFrequency(frequency);
	}

	inline void changeAllVoices(VoiceChangerFunction changer) {
		for (auto i = 0; i < NumberOfVoices; ++i) {
			changer(voices_[i]);
		}
	}

	static void setVolumeEnvelopeStageValue(Voice& voice, EnvelopeGenerator::Stage stage, double value) {
		voice.volumeEnvelope().setStageValue(stage, value);
	}

	static void setFilterEnvelopeStageValue(Voice& voice, EnvelopeGenerator::Stage stage, double value) {
		voice.filterEnvelope().setStageValue(stage, value);
	}

	static void setOscillatorMode(Voice& voice, int oscillatorNumber, Oscillator::Mode mode) {
		switch (oscillatorNumber) {
			case 1:
				voice.oscillator1().setMode(mode);
				break;
			case 2:
				voice.oscillator2().setMode(mode);
				break;
		}
	}

	static void setOscillatorPitchMod(Voice& voice, int oscillatorNumber, double pitchMod) {
		switch (oscillatorNumber) {
			case 1:
				voice.oscillator1().setPitchMod(pitchMod);
				break;
			case 2:
				voice.oscillator2().setPitchMod(pitchMod);
				break;
		}
	}

	static void setOscillatorMix(Voice& voice, double value) {
		voice.setOscillatorMix(value);
	}

	static void setFilterCutoff(Voice& voice, double cutoff) {
		voice.filter().setCutoff(cutoff);
	}

	static void setFilterResonance(Voice& voice, double resonance) {
		voice.filter().setResonance(resonance);
	}

	static void setFilterMode(Voice& voice, Filter::Mode mode) {
		voice.filter().setFilterMode(mode);
	}

	static void setFilterEnvAmount(Voice& voice, double amount) {
		voice.setFilterEnvelopeAmount(amount);
	}

	static void setFilterLfoAmount(Voice& voice, double amount) {
		voice.setFilterLfoAmount(amount);
	}

private:
	static const int NumberOfVoices = 64;
	Voice voices_[NumberOfVoices];
	Oscillator lfo_;
	Voice* findFreeVoice();
};
