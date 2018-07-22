#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IMidiQueue.h"
#include "GallantSignal.h"

using Gallant::Signal2;

class MidiReceiver {
public:
	MidiReceiver();

	Signal2<int, int> noteOn;
	Signal2<int, int> noteOff;

	void advance();
	void onMessageReceived(IMidiMsg* midiMessage);

	inline bool getKeyStatus(int keyIndex) const {
		return keyStatus_[keyIndex]; 
	}

	inline int getNumKeys() const {
		return numKeys_;
	}

	inline void flush(int numFrames) {
		midiQueue_.Flush(numFrames);
		offset_ = 0;
	}

	inline void resize(int blockSize) {
		midiQueue_.Resize(blockSize);
	}

private:
	IMidiQueue midiQueue_;
	static const int keyCount_ = 128;
	int numKeys_;
	bool keyStatus_[keyCount_];
	int offset_;
};
