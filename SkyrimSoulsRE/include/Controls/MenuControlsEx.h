#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class MenuControlsEx : public ConsoleRE::MenuControls
	{
	public:
		ConsoleRE::BSEventNotifyControl ProcessEvent_Hook(ConsoleRE::InputEvent** a_event, ConsoleRE::BSTEventSource<ConsoleRE::InputEvent*>* a_source);

		bool IsMappedToSameButton(uint32_t a_keyMask, ConsoleRE::INPUT_DEVICE a_deviceType, ConsoleRE::BSFixedString a_controlName, ConsoleRE::UserEvents::INPUT_CONTEXT_ID a_context = ConsoleRE::UserEvents::INPUT_CONTEXT_ID::kGameplay);
		static void InstallHook();

		using ProcessEvent_t = decltype(static_cast<ConsoleRE::BSEventNotifyControl (ConsoleRE::MenuControls::*)(ConsoleRE::InputEvent* const*, ConsoleRE::BSTEventSource<ConsoleRE::InputEvent*>*)>(&ConsoleRE::MenuControls::ProcessEvent));
		static inline REL::Relocation<ProcessEvent_t> _ProcessEvent;
	};
}
