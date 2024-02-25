#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kRequiresUpdate | kCustomRendering
	// kUsesCursor if gamepad disabled
	// context = kStats
	class StatsMenuEx : public ConsoleRE::StatsMenu
	{
	public:
		ConsoleRE::UI_MESSAGE_RESULTS ProcessMessage_Hook(ConsoleRE::UIMessage& a_message);	// 04
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);	// 05

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();

		static inline bool isSleeping = false;

		using ProcessMessage_t = decltype(&ConsoleRE::StatsMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using AdvanceMovie_t = decltype(&ConsoleRE::StatsMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}
