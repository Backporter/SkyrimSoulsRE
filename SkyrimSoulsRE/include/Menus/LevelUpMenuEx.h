#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kModal | kDisablePauseMenu
	// kUsesCursor is gamepad is disabled
	// context = kMenuMode
	class LevelUpMenuEx : public ConsoleRE::LevelUpMenu
	{
	public:
		static ConsoleRE::IMenu* Creator();

		static void InstallHook();
	};
}
