#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 4
	// flags = kPausesGame | kRequiresUpdate | kAllowSaving | kApplicationMenu
	// kUsesCursor if gamepad disabled
	// context = kMenuMode
	class SleepWaitMenuEx : public ConsoleRE::SleepWaitMenu
	{
	public:
		ConsoleRE::UI_MESSAGE_RESULTS ProcessMessage_Hook(ConsoleRE::UIMessage& a_message);
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);

		void UpdateClock();

		static void StartSleepWait_Hook(const ConsoleRE::FxDelegateArgs& a_args);

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();

		using AdvanceMovie_t = decltype(&ConsoleRE::SleepWaitMenu::AdvanceMovie);
		using ProcessMessage_t = decltype(&ConsoleRE::SleepWaitMenu::ProcessMessage);

		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		static inline ConsoleRE::FxDelegateHandler::CallbackFn* _StartSleepWait;
	};

}
