#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class BarterMenuEx : public ConsoleRE::BarterMenu
	{
	public:
		ConsoleRE::UI_MESSAGE_RESULTS ProcessMessage_Hook(ConsoleRE::UIMessage& a_message);	//04
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);	// 05

		static void ItemSelect_Hook(const ConsoleRE::FxDelegateArgs& a_args);

		void UpdateBottomBar();

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&ConsoleRE::BarterMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using AdvanceMovie_t = decltype(&ConsoleRE::BarterMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		static inline ConsoleRE::FxDelegateHandler::CallbackFn* _ItemSelect;
	};
}
