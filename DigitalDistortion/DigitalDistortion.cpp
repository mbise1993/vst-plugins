#include "DigitalDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 5;

enum EParams {
	kThreshold = 0,
	kNumParams
};

enum ELayout {
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,

	kThresholdX = 79,
	kThresholdY = 62,
	kKnobFrames = 128
};

DigitalDistortion::DigitalDistortion(IPlugInstanceInfo instanceInfo)
	: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), threshold_(1.) {
	TRACE;

	//arguments are: name, defaultVal, minVal, maxVal, step, label
	GetParam(kThreshold)->InitDouble("Amount", 0.0, 0.0, 99.99, 0.01, "%");
	GetParam(kThreshold)->SetShape(2.);

	IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
	pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

	IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

	pGraphics->AttachControl(new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knob));

	AttachGraphics(pGraphics);

	CreatePresets();
}

DigitalDistortion::~DigitalDistortion() {
}

void DigitalDistortion::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) {
	// Mutex is already locked for us.
	const auto channelCount = 2;

	for (auto i = 0; i < channelCount; ++i) {
		auto input = inputs[i];
		auto output = outputs[i];

		for (auto s = 0; s < nFrames; ++s, ++input, ++output) {
			if (*input >= 0) {
				*output = fmin(*input, threshold_);
			} else {
				*output = fmax(*input, -threshold_);
			}

			*output /= threshold_;
		}
	}
}

void DigitalDistortion::Reset() {
	TRACE;
	IMutexLock lock(this);
}

void DigitalDistortion::OnParamChange(int paramIdx) {
	IMutexLock lock(this);

	switch (paramIdx) {
		case kThreshold:
			threshold_ = 1 - (GetParam(kThreshold)->Value() / 100.0);
			break;

		default:
			break;
	}
}

void DigitalDistortion::CreatePresets() {
	MakePreset("Clean", 100.0);
	MakePreset("Slightly Distorted", 80.0);
	MakePreset("Crunch", 40.0);
	MakePreset("Crush", 20.0);
	MakePreset("Buzz", 0.01);
}
