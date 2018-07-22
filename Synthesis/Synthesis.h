#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "VoiceManager.h"
#include "MidiReceiver.h"

class Synthesis : public IPlug {
public:
	static const int virtualKeyboardMinNoteNumber = 48;
	int lastVirtualKeyboardNoteNumber;

	Synthesis(IPlugInstanceInfo instanceInfo);
	~Synthesis();

	void Reset();
	void OnParamChange(int paramIdx);
	void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
	void ProcessMidiMsg(IMidiMsg* pMsg);

	inline int GetNumKeys() const {
		return midiReceiver_.getNumKeys();
	}

	inline bool GetKeyStatus(int key) const {
		return midiReceiver_.getKeyStatus(key);
	}

private:
	MidiReceiver midiReceiver_;
	IControl* virtualKeyboard_;
	VoiceManager voiceManager_;

	void CreateParams();
	void CreateGraphics();
	void CreatePresets();
	void ProcessVirtualKeyboard();
};
