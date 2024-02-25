#pragma once

//
#include "../../../CSEL/source/REL/Relocation.h"

namespace Offsets
{
	namespace ConsoleCommands 
	{
		static constexpr REL::Offset CenterOnCell_Hook(static_cast<uintptr_t>(0x2BBB20));
		static constexpr REL::Offset CenterOnWorld_Hook(static_cast<uintptr_t>(0x2BBD90));
		static constexpr REL::Offset CenterOnExterior_Hook(static_cast<uintptr_t>(0x2BBBD0));
		static constexpr REL::Offset ServeTime_Hook(static_cast<uintptr_t>(0x2B2CF0));
		static constexpr REL::Offset SaveGame_Hook(static_cast<uintptr_t>(0x2BD0B0));
	}

	namespace BSAudioManager
	{
		static constexpr REL::Offset Hook(static_cast<uintptr_t>(0x7E05D0));
		static constexpr REL::Offset SetListenerPosition(static_cast<uintptr_t>(0xE10DE0));
		static constexpr REL::Offset SetListenerRotation(static_cast<uintptr_t>(0xE10E30));
	}

	namespace GlobalTimescaleMultiplier
	{
		static constexpr REL::Offset Value1(static_cast<uintptr_t>(0x20A9260));
		static constexpr REL::Offset Value2(static_cast<uintptr_t>(0x20A9264));
	}

	namespace ItemMenuUpdater
	{
		static constexpr REL::Offset RequestItemListUpdate(static_cast<uintptr_t>(0xA86250));

		static constexpr REL::Offset RemoveAllItems(static_cast<uintptr_t>(0x1468C0));
		static constexpr REL::Offset RemoveAllItems_Hook1(static_cast<uintptr_t>(0x146890));
		static constexpr REL::Offset RemoveAllItems_Hook2(static_cast<uintptr_t>(0x233830));
		static constexpr REL::Offset RemoveAllItems_Hook3(static_cast<uintptr_t>(0x295570));
		static constexpr REL::Offset RemoveAllItems_Hook4(static_cast<uintptr_t>(0x71FC00));
		static constexpr REL::Offset RemoveAllItems_Hook5(static_cast<uintptr_t>(0xB69CE0));

		static constexpr REL::Offset RemoveItem_Actor(static_cast<uintptr_t>(0x71DC10));
		static constexpr REL::Offset RemoveItem_TESObjectREFR(static_cast<uintptr_t>(0x22CF30));

		static constexpr REL::Offset RemoveItem_Hook1(static_cast<uintptr_t>(21520));  // + 0x9A (6 call) - Actor - when called from Console (Drop)
		static constexpr REL::Offset RemoveItem_Hook2(static_cast<uintptr_t>(0xB6F990));  // + 0xD9 (6 call) - Actor - when called from Payprus (DropObject)
		static constexpr REL::Offset RemoveItem_Hook3(static_cast<uintptr_t>(0x318830));  // + 0x461 (6 call) - Actor - when other NPCs interact
		static constexpr REL::Offset RemoveItem_Hook4(static_cast<uintptr_t>(0x1A3730));  // + 0x268 (6 call) - TESObjectREFR - when other NPCs interact
	}

	namespace Menus
	{
		namespace BarterMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D94410));

			static constexpr REL::Offset UpdateBottomBar(static_cast<uintptr_t>(0x9D9600));
		}

		namespace BookMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D945E0));
		}

		namespace Console
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D94790));
		}

		namespace ContainerMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D948E0));

			static constexpr REL::Offset UpdateBottomBar(static_cast<uintptr_t>(0x9E6AE0));
		}

		namespace DialogueMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D95310));

			static constexpr REL::Offset UpdateAutoCloseTimer_Hook(static_cast<uintptr_t>(0x6F9980));
		}

		namespace FavoritesMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D95460));

			static constexpr REL::Offset CanProcess(static_cast<uintptr_t>(0xA05780));

			static constexpr REL::Offset SelectItem_HookFunc_1(static_cast<uintptr_t>(0xA03B70));
			static constexpr REL::Offset SelectItem_HookFunc_2(static_cast<uintptr_t>(0xA03B00));

			static constexpr REL::Offset ItemSelect(static_cast<uintptr_t>(0xA04810));
		}

		namespace GiftMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D95560));

			static constexpr REL::Offset UpdateBottomBar(static_cast<uintptr_t>(0xA08100));
		}

		namespace HUDMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D956D0));
		}

		namespace InventoryMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D95EE0));

			static constexpr REL::Offset UpdateBottomBar(static_cast<uintptr_t>(0xA23150));
		}

		namespace JournalMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D99500));

			static constexpr REL::Offset RemapHandler_Vtbl(static_cast<uintptr_t>(0x1D994B8));
		}

		namespace LevelUpMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D95FA0));
		}

		namespace LockpickingMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D96120));

			static constexpr REL::Offset Hook(0xA2BE80);
		}

		namespace MagicMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D962B0));

			static constexpr REL::Offset UpdateItemList(static_cast<uintptr_t>(0xA359B0));
			static constexpr REL::Offset AddSpells(static_cast<uintptr_t>(0x74C610));
			static constexpr REL::Offset AddShout(static_cast<uintptr_t>(0xA38F50));
			static constexpr REL::Offset UpdateBottomBar(static_cast<uintptr_t>(0xA36270));
		}

		namespace MapMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D98D80));

			static constexpr REL::Offset Ctor(static_cast<uintptr_t>(0xA95FA0));

			static constexpr REL::Offset LocalMapUpdaterFunc(static_cast<uintptr_t>(0xA97E80));

			namespace MapLookHandler
			{
				static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D98C30));
			}

			namespace MapMoveHandler
			{
				static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D98C98));

			}

			namespace MapZoomHandler
			{
				static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D98CF8));
			}
		}

		namespace MessageBoxMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D96E10));
		}

		namespace ModManagerMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D59F60));
		}

		namespace StatsMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D97480));

			static constexpr REL::Offset ProcessMessage(static_cast<uintptr_t>(0xA657F0));
			static constexpr REL::Offset CanProcess(static_cast<uintptr_t>(0xA6FA70));

			static constexpr REL::Offset OpenStatsMenuAfterSleep_Hook(static_cast<uintptr_t>(0x7EAB70));
		}

		namespace SleepWaitMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D97400));
		}

		namespace TrainingMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D97A40));
		}

		namespace TutorialMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D97B10));
		}

		namespace TweenMenu
		{
			static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D97B90));

			static constexpr REL::Offset Camera_Hook(0xA7B320);
		}
	}

	namespace MenuControls
	{
		static constexpr REL::Offset Vtbl(static_cast<uintptr_t>(0x1D96A40));
	}

	namespace Misc
	{
		static constexpr REL::Offset CreateSaveScreenshot(static_cast<uintptr_t>(0x6BE5F0));
		static constexpr REL::Offset CursorPosition(static_cast<uintptr_t>(0x37EA6F0));
		static constexpr REL::Offset ScreenEdgeCameraMoveHook(static_cast<uintptr_t>(0x85E170));
	}

	namespace Papyrus::IsInMenuMode
	{
		static constexpr REL::Offset Hook(static_cast<uintptr_t>(0xB8A0A0));
		static constexpr REL::Offset Value1(static_cast<uintptr_t>(0x313D8ED));
		static constexpr REL::Offset Value2(static_cast<uintptr_t>(0x313D8F4));
	}

	namespace UnpausedTaskQueue
	{
		static constexpr REL::Offset Hook(static_cast<uintptr_t>(0x6A3800));
		static constexpr REL::Offset OriginalFunc(static_cast<uintptr_t>(0x6A7640));
	}
}
