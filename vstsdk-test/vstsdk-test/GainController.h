#pragma once

#include "vstgui4/vstgui/plugin-bindings/vst3editor.h"
#include "public.sdk/source/vst/vsteditcontroller.h"

namespace sb = Steinberg;

namespace mbise
{
	class GainController : public sb::Vst::EditControllerEx1, public VSTGUI::VST3EditorDelegate {
	public:
		GainController();
		~GainController();
	};
}
