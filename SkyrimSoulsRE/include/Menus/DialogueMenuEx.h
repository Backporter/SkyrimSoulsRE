#pragma once

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kUpdateUsesCursor | kDontHideCursorWhenTopmost
	// context = kMenuMode
	class DialogueMenuEx : public ConsoleRE::DialogueMenu
	{
	public:
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);	// 05

		static void UpdateAutoCloseTimer_Hook(ConsoleRE::AIProcess* a_process, float a_delta);	 // This runs every frame

		static void InstallHook();

		using AdvanceMovie_t = decltype(&ConsoleRE::DialogueMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}
