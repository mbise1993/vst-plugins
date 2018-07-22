#include "MidiReceiver.h"

MidiReceiver::MidiReceiver() :
	numKeys_(0),
	offset_(0) {

	for (auto i = 0; i < keyCount_; ++i) {
		keyStatus_[i] = false;
	}
}

void MidiReceiver::onMessageReceived(IMidiMsg* midiMessage) {
	auto status = midiMessage->StatusMsg();
	if (status == IMidiMsg::kNoteOn || status == IMidiMsg::kNoteOff) {
		midiQueue_.Add(midiMessage);
	}
}

void MidiReceiver::advance() {
	while (!midiQueue_.Empty()) {
		auto midiMessage = midiQueue_.Peek();
		if (midiMessage->mOffset > offset_) {
			break;
		}

		auto status = midiMessage->StatusMsg();
		auto noteNumber = midiMessage->NoteNumber();
		auto velocity = midiMessage->Velocity();

		if (status == IMidiMsg::kNoteOn && velocity) {
			if (keyStatus_[noteNumber] == false) {
				keyStatus_[noteNumber] = true;
				numKeys_++;
				noteOn(noteNumber, velocity);
			}
		} else {
			if (keyStatus_[noteNumber] == true) {
				keyStatus_[noteNumber] = false;
				numKeys_--;
				noteOff(noteNumber, velocity);
			}
		}

		midiQueue_.Remove();
	}

	offset_++;
}