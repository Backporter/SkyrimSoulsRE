#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 10
	// flags = kPausesGame | kModal | kUpdateUsesCursor
	// context = kMenuMode

	class TutorialMenuEx : ConsoleRE::TutorialMenu
	{
	public:
		static ConsoleRE::IMenu* Creator();
		static void InstallHook();
	};
}
