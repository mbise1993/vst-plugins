#include "GainEffect.h"
#include "ClassIds.h"
#include "Version.h"
#include "public.sdk/source/main/pluginfactoryvst3.h"

#define STR_PLUGIN_NAME "Gain VST3"

using namespace mbise;
using namespace Steinberg::Vst;

bool initModule() {
	return true;
}

bool deinitModule() {
	return true;
}

BEGIN_FACTORY_DEF("Matt Bise", "www.github.com/mbise1993", "mbise1993@gmail.com")
	DEF_CLASS2(INLINE_UID_FROM_FUID(ClassIds::ProcessorUid),
		PClassInfo::kManyInstances,
		kVstAudioEffectClass,
		STR_PLUGIN_NAME,
		Vst::kDistributable,
		"Fx",
		FULL_VERSION_STR,
		kVstVersionString,
		GainEffect::createInstance)

END_FACTORY
