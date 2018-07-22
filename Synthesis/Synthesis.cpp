#include "Synthesis.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "IKeyboardControl.h"
#include "resource.h"

const int kNumPrograms = 5;
const double parameterStep = 0.001;

enum EParams {
	// Oscillator params
	kOsc1Waveform = 0,
	kOsc1PitchMod,
	kOsc2Waveform,
	kOsc2PitchMod,
	kOscMix,
	// Filter params
	kFilterMode,
	kFilterCutoff,
	kFilterResonance,
	kFilterLfoAmount,
	kFilterEnvAmount,
	// LFO params
	kLFOWaveform,
	kLFOFrequency,
	// Volume env params
	kVolumeEnvAttack,
	kVolumeEnvDecay,
	kVolumeEnvSustain,
	kVolumeEnvRelease,
	// Filter env params
	kFilterEnvAttack,
	kFilterEnvDecay,
	kFilterEnvSustain,
	kFilterEnvRelease,
	kNumParams
};

typedef struct {
	const char* name;
	const int x;
	const int y;
	const double defaultVal;
	const double minVal;
	const double maxVal;
} ParamProps_t;

const ParamProps_t paramProps[kNumParams] = {
	{"Osc 1 Waveform", 30, 75},
	{"Osc 1 Pitch Mod", 69, 61, 0.0, 0.0, 1.0},
	{"Osc 2 Waveform", 203, 75},
	{"Osc 2 Pitch Mod", 242, 61, 0.0, 0.0, 1.0},
	{"Osc Mix", 130, 61, 0.5, 0.0, 1.0},
	{"Filter Mode", 30, 188},
	{"Filter Cutoff", 69, 174, 0.99, 0.0, 0.99},
	{"Filter Resonance", 124, 174, 0.0, 0.0, 1.0},
	{"Filter LFO Amount", 179, 174, 0.0, 0.0, 1.0},
	{"Filter Envelope Amount", 234, 174, 0.0, -1.0, 1.0},
	{"LFO Waveform", 30, 298},
	{"LFO Frequency", 69, 284, 6.0, 0.01, 30.0},
	{"Volume Env Attack", 323, 61, 0.01, 0.01, 10.0},
	{"Volume Env Decay", 378, 61, 0.5, 0.01, 15.0},
	{"Volume Env Sustain", 433, 61, 0.1, 0.001, 1.0},
	{"Volume Env Release", 488, 61, 1.0, 0.01, 15.0},
	{"Filter Env Attack", 323, 174, 0.01, 0.01, 10.0},
	{"Filter Env Decay", 378, 174, 0.5, 0.01, 15.0},
	{"Filter Env Sustain", 433, 174, 0.1, 0.001, 1.0},
	{"Filter Env Release", 488, 174, 1.0, 0.01, 15.0}
};

enum ELayout {
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,
	kKeybX = 62,
	kKeybY = 425
};

Synthesis::Synthesis(IPlugInstanceInfo instanceInfo) : 
	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
	lastVirtualKeyboardNoteNumber(virtualKeyboardMinNoteNumber - 1) {
	
	TRACE;

	CreateParams();
	CreateGraphics();
	CreatePresets();

	midiReceiver_.noteOn.Connect(&voiceManager_, &VoiceManager::onNoteOn);
	midiReceiver_.noteOff.Connect(&voiceManager_, &VoiceManager::onNoteOff);
}

Synthesis::~Synthesis() = default;

void Synthesis::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) {
	// Mutex is already locked for us.
	auto leftOutput = outputs[0];
	auto rightOutput = outputs[1];

	ProcessVirtualKeyboard();

	for (auto i = 0; i < nFrames; ++i) {
		midiReceiver_.advance();
		leftOutput[i] = rightOutput[i] = voiceManager_.nextSample();
	}

	midiReceiver_.flush(nFrames);
}

void Synthesis::ProcessMidiMsg(IMidiMsg* pMsg) {
	midiReceiver_.onMessageReceived(pMsg);
	virtualKeyboard_->SetDirty();
}

void Synthesis::ProcessVirtualKeyboard() {
	auto virtualKeyboard = (IKeyboardControl*)virtualKeyboard_;
	auto virtualKeyboardNoteNumber = virtualKeyboard->GetKey() + virtualKeyboardMinNoteNumber;

	// Release last key if no longer pressed
	if (lastVirtualKeyboardNoteNumber >= virtualKeyboardMinNoteNumber && virtualKeyboardNoteNumber != lastVirtualKeyboardNoteNumber) {
		IMidiMsg midiMessage;
		midiMessage.MakeNoteOffMsg(lastVirtualKeyboardNoteNumber, 0);
		midiReceiver_.onMessageReceived(&midiMessage);
	}

	// Handle new, valid keypress
	if (virtualKeyboardNoteNumber >= virtualKeyboardMinNoteNumber && virtualKeyboardNoteNumber != lastVirtualKeyboardNoteNumber) {
		IMidiMsg midiMessage;
		midiMessage.MakeNoteOnMsg(virtualKeyboardNoteNumber, virtualKeyboard->GetVelocity(), 0);
		midiReceiver_.onMessageReceived(&midiMessage);
	}

	lastVirtualKeyboardNoteNumber = virtualKeyboardNoteNumber;
}

void Synthesis::Reset() {
	TRACE;
	IMutexLock lock(this);
	voiceManager_.setSampleRate(GetSampleRate());
}

void Synthesis::OnParamChange(int paramIdx) {
	IMutexLock lock(this);
	auto param = GetParam(paramIdx);
	if (paramIdx == kLFOWaveform) {
		voiceManager_.setLfoMode(static_cast<Oscillator::Mode>(param->Int()));
	} else if (paramIdx == kLFOFrequency) {
		voiceManager_.setLfoFrequency(param->Value());
	} else {
		using std::placeholders::_1;
		using std::bind;
		VoiceManager::VoiceChangerFunction changer;
		switch (paramIdx) {
			case kOsc1Waveform:
				changer = bind(&VoiceManager::setOscillatorMode, _1, 1, static_cast<Oscillator::Mode>(param->Int()));
				break;
			case kOsc1PitchMod:
				changer = bind(&VoiceManager::setOscillatorPitchMod, _1, 1, param->Value());
				break;
			case kOsc2Waveform:
				changer = bind(&VoiceManager::setOscillatorMode, _1, 2, static_cast<Oscillator::Mode>(param->Int()));
				break;
			case kOsc2PitchMod:
				changer = bind(&VoiceManager::setOscillatorPitchMod, _1, 2, param->Value());
				break;
			case kOscMix:
				changer = bind(&VoiceManager::setOscillatorMix, _1, param->Value());
				break;
				// Filter Section:
			case kFilterMode:
				changer = bind(&VoiceManager::setFilterMode, _1, static_cast<Filter::Mode>(param->Int()));
				break;
			case kFilterCutoff:
				changer = bind(&VoiceManager::setFilterCutoff, _1, param->Value());
				break;
			case kFilterResonance:
				changer = bind(&VoiceManager::setFilterResonance, _1, param->Value());
				break;
			case kFilterLfoAmount:
				changer = bind(&VoiceManager::setFilterLfoAmount, _1, param->Value());
				break;
			case kFilterEnvAmount:
				changer = bind(&VoiceManager::setFilterEnvAmount, _1, param->Value());
				break;
				// Volume Envelope:
			case kVolumeEnvAttack:
				changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, param->Value());
				break;
			case kVolumeEnvDecay:
				changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_DECAY, param->Value());
				break;
			case kVolumeEnvSustain:
				changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN, param->Value());
				break;
			case kVolumeEnvRelease:
				changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, param->Value());
				break;
				// Filter Envelope:
			case kFilterEnvAttack:
				changer = bind(&VoiceManager::setFilterEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, param->Value());
				break;
			case kFilterEnvDecay:
				changer = bind(&VoiceManager::setFilterEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_DECAY, param->Value());
				break;
			case kFilterEnvSustain:
				changer = bind(&VoiceManager::setFilterEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN, param->Value());
				break;
			case kFilterEnvRelease:
				changer = bind(&VoiceManager::setFilterEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, param->Value());
				break;
		}

		voiceManager_.changeAllVoices(changer);
	}
}

void Synthesis::CreateParams() {
	for (auto i = 0; i < kNumParams; ++i) {
		auto param = GetParam(i);
		const auto& props = paramProps[i];
		switch (i) {
			case kOsc1Waveform:
			case kOsc2Waveform:
				param->InitEnum(props.name, Oscillator::OSCILLATOR_MODE_SAW, Oscillator::kNumOscillatorModes);
				param->SetDisplayText(0, props.name); // for VST3
				break;
			case kLFOWaveform:
				param->InitEnum(props.name, Oscillator::OSCILLATOR_MODE_TRIANGLE, Oscillator::kNumOscillatorModes);
				param->SetDisplayText(0, props.name); // for VST3
				break;
			case kFilterMode:
				param->InitEnum(props.name, Filter::FILTER_MODE_LOWPASS, Filter::kNumFilterModes);
				break;
			// All double parameters
			default:
				param->InitDouble(props.name, props.defaultVal, props.minVal, props.maxVal, parameterStep);
				break;
		}
	}

	GetParam(kFilterCutoff)->SetShape(2);
	GetParam(kVolumeEnvAttack)->SetShape(3);
	GetParam(kFilterEnvAttack)->SetShape(3);
	GetParam(kVolumeEnvDecay)->SetShape(3);
	GetParam(kFilterEnvDecay)->SetShape(3);
	GetParam(kVolumeEnvSustain)->SetShape(2);
	GetParam(kFilterEnvSustain)->SetShape(2);
	GetParam(kVolumeEnvRelease)->SetShape(3);
	GetParam(kFilterEnvRelease)->SetShape(3);

	// Ensure plugin gets correct initial values when loaded
	for (auto i = 0; i < kNumParams; ++i) {
		OnParamChange(i);
	}
}

void Synthesis::CreateGraphics() {
	auto pGraphics = MakeGraphics(this, kWidth, kHeight);
	pGraphics->AttachBackground(BG_ID, BG_FN);

	// Create virtual keyboard
	auto whiteKeyImage = pGraphics->LoadIBitmap(WHITE_KEY_ID, WHITE_KEY_FN, 6);
	auto blackKeyImage = pGraphics->LoadIBitmap(BLACK_KEY_ID, BLACK_KEY_FN);
	int keyCoordinates[12] = {0, 10, 17, 30, 35, 52, 61, 68, 79, 85, 97, 102};
	virtualKeyboard_ = new IKeyboardControl(
		this, kKeybX, kKeybY,
		virtualKeyboardMinNoteNumber,
		/* octaves */ 4,
		&whiteKeyImage, &blackKeyImage,
		keyCoordinates);

	// Load knob and switch images
	auto waveformBitmap = pGraphics->LoadIBitmap(WAVEFORM_ID, WAVEFORM_FN, 4);
	auto filterModeBitmap = pGraphics->LoadIBitmap(FILTERMODE_ID, FILTERMODE_FN, 3);
	auto knobBitmap = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, 64);

	for (auto i = 0; i < kNumParams; ++i) {
		const auto& props = paramProps[i];
		IControl* control;
		IBitmap* graphic;
		switch (i) {
			// Switches
			case kOsc1Waveform:
			case kOsc2Waveform:
			case kLFOWaveform:
				graphic = &waveformBitmap;
				control = new ISwitchControl(this, props.x, props.y, i, graphic);
				break;
			case kFilterMode:
				graphic = &filterModeBitmap;
				control = new ISwitchControl(this, props.x, props.y, i, graphic);
				break;
			// Knobs
			default:
				graphic = &knobBitmap;
				control = new IKnobMultiControl(this, props.x, props.y, i, graphic);
				break;
		}

		pGraphics->AttachControl(control);
	}

	AttachGraphics(pGraphics);
}

void Synthesis::CreatePresets() {
	
}
