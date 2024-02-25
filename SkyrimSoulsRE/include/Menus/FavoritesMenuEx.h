#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kFavorites
	class FavoritesMenuEx :
		public ConsoleRE::FavoritesMenu  // 00
	{
	public:
		static void ItemSelect_Hook(ConsoleRE::FavoritesMenu* a_this, ConsoleRE::BGSEquipSlot* a_slot);

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();

		static inline ConsoleRE::FxDelegateHandler::CallbackFn* _ItemSelect;
	};
}
