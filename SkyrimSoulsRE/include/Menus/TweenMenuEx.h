#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 0
	// flags = kPausesGame | kUpdateUsesCursor | kAllowSaving | kDontHideCursorWhenTopmost | kCustomRendering
	// context = kMenuMode
	class TweenMenuEx : public ConsoleRE::TweenMenu
	{
	public:
		// override (IMenu)
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);	// 05

		void UpdateClock();

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();

		using AdvanceMovie_t = decltype(&ConsoleRE::TweenMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}
