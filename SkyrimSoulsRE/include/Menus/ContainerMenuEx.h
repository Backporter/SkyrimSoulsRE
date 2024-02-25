#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 0
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kItemMenu
	class ContainerMenuEx : public ConsoleRE::ContainerMenu
	{
	public:
		enum ContainerMode : uint8_t
		{
			kLoot = 0,
			kSteal = 1,
			kPickpocket = 2,
			kNPC = 3
		};

		static inline ConsoleRE::TESObjectREFR* containerRef;
		static inline std::wstring toSteal;
		static inline std::wstring toPlace;

		// Virtual functions
		ConsoleRE::UI_MESSAGE_RESULTS ProcessMessage_Hook(ConsoleRE::UIMessage& a_message);	// 04
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);	// 05

		// FxDelegate callbacks
		static void TransferItem_Hook(const ConsoleRE::FxDelegateArgs& a_args);
		static void EquipItem_Hook(const ConsoleRE::FxDelegateArgs& a_args);
		static void TakeAllItems_Hook(const ConsoleRE::FxDelegateArgs& a_args);

		void UpdatePickpocketChance();
		void UpdateBottomBar();

		int32_t CalcPickPocketChance(ConsoleRE::StandardItemData* a_itemData);

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();
		static void ParseTranslations();

		using AdvanceMovie_t = decltype(&ConsoleRE::ContainerMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		using ProcessMessage_t = decltype(&ConsoleRE::ContainerMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		static inline ConsoleRE::FxDelegateHandler::CallbackFn* _TransferItem;
		static inline ConsoleRE::FxDelegateHandler::CallbackFn* _EquipItem;
		static inline ConsoleRE::FxDelegateHandler::CallbackFn* _TakeAllItems;
	};
}
