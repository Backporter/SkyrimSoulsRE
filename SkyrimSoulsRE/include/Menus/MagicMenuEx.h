#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 0
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kItemMenu
	class MagicMenuEx : public ConsoleRE::MagicMenu
	{
	public:
		ConsoleRE::UI_MESSAGE_RESULTS ProcessMessage_Hook(ConsoleRE::UIMessage& a_message);	// 04
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);	// 05

		bool IsViewingActiveEffects();
		void UpdateBottomBar();

		static void AddSpells_Hook(ConsoleRE::PlayerCharacter* a_player, void* a_unk1);
		static void AddShouts_Hook(ConsoleRE::ItemList* a_list, void* a_unk1, uint32_t a_playerRefHandle);

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();

		static inline bool blockSpells = false;
		static inline bool wasViewingActiveEffects = false;

		using ProcessMessage_t = decltype(&ConsoleRE::MagicMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using AdvanceMovie_t = decltype(&ConsoleRE::MagicMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}
