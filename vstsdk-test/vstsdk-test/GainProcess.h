#pragma once

namespace mbise
{
	template<typename TSample>
	void GainEffect::processAudio(TSample** in, TSample** out, sb::int32 numChannels, sb::int32 sampleFrames, float gain) {
		for (auto i = 0; i < numChannels; ++i) {
			auto samples = sampleFrames;
			auto ptrIn = (TSample*)in[i];
			auto ptrOut = (TSample*)out[i];
			TSample temp;
			while (--samples >= 0) {
				temp = (*ptrIn++) * gain;
				(*ptrOut++) = temp;
			}
		}
	}
}
