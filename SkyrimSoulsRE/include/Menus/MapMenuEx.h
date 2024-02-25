#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kUsesCursor | kRendersOffscreenTargets | kCustomRendering
	// context = kMap
	class MapMenuEx : public ConsoleRE::MapMenu
	{
	public:
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);	// 05
		ConsoleRE::UI_MESSAGE_RESULTS ProcessMessage_Hook(ConsoleRE::UIMessage& a_message);	// 04

		void UpdateClock();

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();

		inline static bool closeMenu = false;
		inline static bool restoreAutoMove = false;

		using ProcessMessage_t = decltype(&ConsoleRE::MapMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using AdvanceMovie_t = decltype(&ConsoleRE::MapMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}
