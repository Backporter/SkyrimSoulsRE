#pragma once

#include "AudioHooks.h"
#include "ConsoleCommandHooks.h"
#include "ItemMenuUpdater.h"
#include "Offsets.h"
#include "PapyrusHook.h"
#include "Settings.h"
#include "SlowMotionHandler.h"
#include "UnpausedTaskQueue.h"

#include "AutoCloseManager.h"

#include "Menus/BarterMenuEx.h"
#include "Menus/BookMenuEx.h"
#include "Menus/CombatAlertOverlayMenu.h"
#include "Menus/ConsoleEx.h"
#include "Menus/ContainerMenuEx.h"
#include "Menus/CustomMenuEx.h"
#include "Menus/DialogueMenuEx.h"
#include "Menus/FavoritesMenuEx.h"
#include "Menus/GiftMenuEx.h"
#include "Menus/HUDMenuEx.h"
#include "Menus/InventoryMenuEx.h"
#include "Menus/JournalMenuEx.h"
#include "Menus/LevelUpMenuEx.h"
#include "Menus/LockpickingMenuEx.h"
#include "Menus/MagicMenuEx.h"
#include "Menus/MapMenuEx.h"
#include "Menus/MessageBoxMenuEx.h"
#include "Menus/ModManagerMenuEx.h"
#include "Menus/SleepWaitMenuEx.h"
#include "Menus/StatsMenuEx.h"
#include "Menus/TrainingMenuEx.h"
#include "Menus/TutorialMenuEx.h"
#include "Menus/TweenMenuEx.h"

#include "Controls/CameraMovement.h"
#include "Controls/InputHandlerEx.h"
#include "Controls/MapInputHandlerEx.h"
#include "Controls/MenuControlsEx.h"
#include "Controls/PlayerControlsEx.h"

namespace SkyrimSoulsRE
{
	enum class MenuFlagEx : uint32_t
	{
		kUnpaused = 1 << 28,
		kUsesCombatAlertOverlay = 1 << 29,
		kUsesSlowMotion = 1 << 30
	};

	ConsoleRE::IMenu* CreateMenu(const std::string& a_menuName);
	uint32_t GetCombatAlertOverlayCount();
	uint32_t GetUnpausedMenuCount();
	uint32_t GetSlowMotionCount();
	bool IsFullScreenMenuOpen();

	void InstallMenuHooks();
	void InstallHooks();
}
