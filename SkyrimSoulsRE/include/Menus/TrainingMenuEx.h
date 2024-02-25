#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kUsesMenuContext | kUpdateUsesCursor
	// kUsesCursor if gamepad disabled
	// context = kNone
	class TrainingMenuEx : public ConsoleRE::TrainingMenu
	{
	public:
		static ConsoleRE::IMenu* Creator();
		static void InstallHook();
	};
}
