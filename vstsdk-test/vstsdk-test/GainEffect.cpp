#include "GainEffect.h"
#include "GainProcess.h"
#include "ClassIds.h"
#include "ParameterIds.h"

#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstevents.h"

namespace mbise
{
	GainEffect::GainEffect() :
		gain_(1.0f),
		isBypassed_(false) {

		setControllerClass(ClassIds::ControllerUid);
	}


	GainEffect::~GainEffect() = default;

	sb::FUnknown* GainEffect::createInstance(void* context) {
		return (sb::Vst::IAudioProcessor*)new GainEffect();
	}

	sb::tresult PLUGIN_API GainEffect::initialize(sb::FUnknown* context) {
		// init parent
		auto result = sb::Vst::AudioEffect::initialize(context);
		if (result != sb::kResultOk) {
			return result;
		}

		// create stereo in and out busses for audio
		addAudioInput(STR16("Stereo In"), sb::Vst::SpeakerArr::kStereo);
		addAudioOutput(STR16("Stereo Out"), sb::Vst::SpeakerArr::kStereo);
		
		// create single event bus
		addEventInput(STR16("Event In"), 1);

		return sb::kResultOk;
	}

	sb::tresult PLUGIN_API GainEffect::setBusArrangements(sb::Vst::SpeakerArrangement* inputs, sb::int32 numIns,
		sb::Vst::SpeakerArrangement* outputs, sb::int32 numOuts) {

		auto inputChannelCount = sb::Vst::SpeakerArr::getChannelCount(inputs[0]);
		auto outputChannelCount = sb::Vst::SpeakerArr::getChannelCount(outputs[0]);

		if (numIns == 1 && numOuts == 1 && inputChannelCount == 1 && outputChannelCount == 1) {
			if (auto bus = sb::FCast<sb::Vst::AudioBus>(audioInputs.at(0))) {
				if (bus->getArrangement() != inputs[0]) {
					removeAudioBusses();
					addAudioInput(STR16("Mono In"), inputs[0]);
					addAudioOutput(STR16("Mono Out"), inputs[0]);
				}

				return sb::kResultOk;
			}
		} else {
			if (auto bus = sb::FCast<sb::Vst::AudioBus>(audioInputs.at(0))) {
				if (inputChannelCount == 2 && outputChannelCount == 2) {
					removeAudioBusses();
					addAudioInput(STR16("Stereo In"), inputs[0]);
					addAudioOutput(STR16("Stereo Out"), inputs[0]);
					return sb::kResultTrue;
				} else if (bus->getArrangement() != sb::Vst::SpeakerArr::kStereo) {
					removeAudioBusses();
					addAudioInput(STR16("Stereo In"), sb::Vst::SpeakerArr::kStereo);
					addAudioOutput(STR16("Stereo Out"), sb::Vst::SpeakerArr::kStereo);
					return sb::kResultFalse;
				}
			}
		}

		return sb::kResultFalse;
	}

	sb::tresult PLUGIN_API GainEffect::canProcessSampleSize(sb::int32 symbolicSampleSize) {
		if (symbolicSampleSize == sb::Vst::kSample32 || symbolicSampleSize == sb::Vst::kSample64) {
			return sb::kResultTrue;
		}

		return sb::kResultFalse;
	}

	sb::tresult PLUGIN_API GainEffect::process(sb::Vst::ProcessData& data) {
		handleParamChanges(data);

		if (data.numInputs == 0 || data.numOutputs == 0) {
			return sb::kResultOk;
		}

		// Get audio buffers
		auto numChannels = data.inputs[0].numChannels;
		auto sampleFrameSize = getSampleFramesSizeInBytes(data.numSamples);
		auto in = getChannelBuffersPointer(data.inputs[0]);
		auto out = getChannelBuffersPointer(data.outputs[0]);

		// Check if silence
		if (data.inputs[0].silenceFlags != 0) {
			// Mark outputs silence too
			data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

			// Ensure output buffer is clear
			for (auto i = 0; i < numChannels; ++i) {
				if (in[i] != out[i]) {
					memset(out[i], 0, sampleFrameSize);
				}
			}

			// Nothing to do
			return sb::kResultOk;
		}

		// Mark outputs not silent
		data.outputs[0].silenceFlags = 0;

		// If in bypass, outputs should equal inputs
		if (isBypassed_) {
			for (auto i = 0; i < numChannels; ++i) {
				if (in[i] != out[i]) {
					memcpy(out[i], in[i], sampleFrameSize);
				}
			}
		} else {
			if (data.symbolicSampleSize == sb::Vst::kSample32) {
				processAudio<sb::Vst::Sample32>(
					(sb::Vst::Sample32**)in, 
					(sb::Vst::Sample32**)out, 
					numChannels, data.numSamples, gain_);
			} else {
				processAudio<sb::Vst::Sample64>(
					(sb::Vst::Sample64**)in,
					(sb::Vst::Sample64**)out,
					numChannels, data.numSamples, gain_);
			}
		}

		return sb::kResultOk;
	}

	void GainEffect::handleParamChanges(sb::Vst::ProcessData& data) {
		if (auto paramChanges = data.inputParameterChanges) {
			auto numChanged = paramChanges->getParameterCount();
			for (auto i = 0; i < numChanged; ++i) {
				auto paramQueue = paramChanges->getParameterData(i);
				if (!paramQueue) {
					continue;
				}

				sb::Vst::ParamValue value;
				sb::int32 sampleOffset;
				auto numPoints = paramQueue->getPointCount();
				switch (paramQueue->getParameterId()) {
					case PARAM_GAIN_ID:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == sb::kResultTrue) {
							gain_ = (float)value;
						}
						break;
					case PARAM_BYPASS_ID:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == sb::kResultTrue) {
							isBypassed_ = value > 0.5;
						}
						break;
				}
			}
		}
	}
}
