#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class MessageBoxMenuEx : public ConsoleRE::MessageBoxMenu
	{
	public:
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();

		static void ButtonPress_Hook(const ConsoleRE::FxDelegateArgs& a_args);

		static inline ConsoleRE::FxDelegateHandler::CallbackFn* _ButtonPress;

		using AdvanceMovie_t = decltype(&ConsoleRE::MessageBoxMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}