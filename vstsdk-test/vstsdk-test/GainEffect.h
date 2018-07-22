#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

namespace sb = Steinberg;

namespace mbise
{
	class GainEffect : public sb::Vst::AudioEffect {
	public:
		GainEffect();
		virtual ~GainEffect();

		static sb::FUnknown* createInstance(void* context);

		sb::tresult PLUGIN_API initialize(sb::FUnknown* context) override;
		sb::tresult PLUGIN_API setBusArrangements(sb::Vst::SpeakerArrangement* inputs, sb::int32 numIns,
			sb::Vst::SpeakerArrangement* outputs, sb::int32 numOuts) override;
		sb::tresult PLUGIN_API canProcessSampleSize(sb::int32 symbolicSampleSize) override;
		sb::tresult PLUGIN_API process(sb::Vst::ProcessData& data) override;

	private:
		float gain_;
		bool isBypassed_;

		template<typename TSample>
		void processAudio(TSample** input, TSample** output, sb::int32 numChannels, sb::int32 sampleFrames, float gain);

		void handleParamChanges(sb::Vst::ProcessData& data);
	};
}

