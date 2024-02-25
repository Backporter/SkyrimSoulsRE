#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 0
	// flags = kPausesGame | kDisablePauseMenu | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kNone
	class InventoryMenuEx : public ConsoleRE::InventoryMenu
	{
	public:
		ConsoleRE::UI_MESSAGE_RESULTS ProcessMessage_Hook(ConsoleRE::UIMessage& a_message);	// 04
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);	// 05

		void UpdateBottomBar();

		static void ItemDrop_Hook(const ConsoleRE::FxDelegateArgs& a_args);
		static void ItemSelect_Hook(const ConsoleRE::FxDelegateArgs& a_args);

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&ConsoleRE::InventoryMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using AdvanceMovie_t = decltype(&ConsoleRE::InventoryMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		static inline ConsoleRE::FxDelegateHandler::CallbackFn* _ItemSelect;
		static inline ConsoleRE::FxDelegateHandler::CallbackFn* _ItemDrop;
	};
}
