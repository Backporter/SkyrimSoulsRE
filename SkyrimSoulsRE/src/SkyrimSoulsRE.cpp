#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	using Flag = ConsoleRE::IMenu::Flag;

	std::map<std::string, ConsoleRE::UI::Create_t*> menuCreatorMap;

	//Used to make sure that the menu can't open with the cursor right at the edge of the screen
	void CheckCursorPosition()
	{
		ConsoleRE::NiPoint2* cursorPosition = reinterpret_cast<ConsoleRE::NiPoint2*>(Offsets::Misc::CursorPosition.address());
		auto* pref = ConsoleRE::INIPrefSettingCollection::GetSingleton();

		uint32_t resX = pref->GetSetting("iSize W:DISPLAY")->GetUInt32();
		uint32_t resY = pref->GetSetting("iSize H:DISPLAY")->GetUInt32();

		if (cursorPosition->x < 10.0f)
		{
			cursorPosition->x = 10.0f;
		}
		else if (cursorPosition->x > resX - 10.0f)
		{
			cursorPosition->x = resX - 10.0f;
		}

		if (cursorPosition->y < 10.0f)
		{
			cursorPosition->y = 10.0f;
		}
		else if (cursorPosition->y > resY - 10.0f)
		{
			cursorPosition->x = resY - 10.0f;
		}
	}

	ConsoleRE::IMenu* CreateMenu(const std::string& a_menuName)
	{	
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		ConsoleRE::UIBlurManager* blurManager = ConsoleRE::UIBlurManager::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		ConsoleRE::IMenu* menu = menuCreatorMap.find(a_menuName)->second();

		CheckCursorPosition();

		bool isConsole = a_menuName == ConsoleRE::Console::MENU_NAME;
		bool isUnpaused = settings->unpausedMenus[a_menuName];
		bool usesOverlay = isConsole ? false : settings->overlayMenus[a_menuName];

		if (menu->PausesGame())
		{
			if (isUnpaused)
			{
				menu->menuFlags &= static_cast<uint32_t>(~std::underlying_type_t<Flag>(Flag::kPausesGame));

				if (!isConsole)
				{
					bool usesSlowMotion = settings->slowMotionMenus[a_menuName];

					if (usesSlowMotion)
					{
						menu->menuFlags |= static_cast<uint32_t>(MenuFlagEx::kUsesSlowMotion);
					}

					menu->menuFlags |= static_cast<uint32_t>(MenuFlagEx::kUnpaused);
				}
			}

			ConsoleRE::PlayerCharacter::GetSingleton()->InterruptCast(true);
		}

		if (usesOverlay && a_menuName != ConsoleRE::HUDMenu::MENU_NAME)
		{
			menu->menuFlags |= static_cast<uint32_t>(MenuFlagEx::kUsesCombatAlertOverlay);
			if (!ui->IsMenuOpen(CombatAlertOverlayMenu::MENU_NAME))
			{
				ConsoleRE::UIMessageQueue* msgQueue = ConsoleRE::UIMessageQueue::GetSingleton();
				msgQueue->AddMessage(CombatAlertOverlayMenu::MENU_NAME, ConsoleRE::UI_MESSAGE_TYPE::kShow, nullptr);
			}
		}

		if (menu->FreezeFrameBackground() && isUnpaused)
		{
			menu->menuFlags &= static_cast<uint32_t>(~std::underlying_type_t<Flag>(Flag::kFreezeFrameBackground));
		}

		if (menu->InventoryItemMenu() && a_menuName != ConsoleRE::FavoritesMenu::MENU_NAME)
		{
			menu->depthPriority = 1;
		}
		
		if (settings->disableBlur)
		{
			if (blurManager->blurCount > 0)
			{
				blurManager->DecrementBlurCount();
			}
			else if (a_menuName == ConsoleRE::TweenMenu::MENU_NAME)
			{
				blurManager->blurCount--;
			}
		}

		return menu;
	}

	uint32_t GetCombatAlertOverlayCount()
	{
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

		uint32_t count = 0;

		for (auto & it : ui->menuStack)
		{
			ConsoleRE::IMenu* menu = it.get();
			if ((menu->menuFlags & static_cast<uint32_t>(MenuFlagEx::kUsesCombatAlertOverlay)) != static_cast<uint32_t>(Flag::kNone))
			{
				++count;
			}
		}

		return count;
	}

	uint32_t GetUnpausedMenuCount()
	{
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

		uint32_t count = 0;

		for (auto & it : ui->menuStack)
		{
			ConsoleRE::IMenu* menu = it.get();
			if ((menu->menuFlags & static_cast<uint32_t>(MenuFlagEx::kUnpaused)) != static_cast<uint32_t>(Flag::kNone))
			{
				++count;
			}
		}

		return count;
	}

	uint32_t GetSlowMotionCount()
	{
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

		uint32_t count = 0;

		for (auto& it : ui->menuStack)
		{
			ConsoleRE::IMenu* menu = it.get();
			if ((menu->menuFlags & static_cast<uint32_t>(MenuFlagEx::kUsesSlowMotion)) != static_cast<uint32_t>(Flag::kNone))
			{
				++count;
			}
		}

		return count;
	}

	bool IsFullScreenMenuOpen()
	{
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		return ui->IsMenuOpen(ConsoleRE::MapMenu::MENU_NAME) || ui->IsMenuOpen(ConsoleRE::StatsMenu::MENU_NAME);
	}

	void InstallMenuHooks()
	{
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		ui->GetEventSource<ConsoleRE::MenuOpenCloseEvent>()->AddEventSink(&g_slowMotionHandler);

		menuCreatorMap.emplace(ConsoleRE::ContainerMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::ContainerMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::InventoryMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::InventoryMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::MagicMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::MagicMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::TweenMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::TweenMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::SleepWaitMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::SleepWaitMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::BarterMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::BarterMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::GiftMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::GiftMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::FavoritesMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::FavoritesMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::TrainingMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::TrainingMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::TutorialMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::TutorialMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::LockpickingMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::LockpickingMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::BookMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::BookMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::Console::MENU_NAME, ui->menuMap.find(ConsoleRE::Console::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::JournalMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::JournalMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::MessageBoxMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::MessageBoxMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::ModManagerMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::ModManagerMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::HUDMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::HUDMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::MapMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::MapMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::StatsMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::StatsMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace(ConsoleRE::LevelUpMenu::MENU_NAME, ui->menuMap.find(ConsoleRE::LevelUpMenu::MENU_NAME)->m_item.second.create);
		menuCreatorMap.emplace("CustomMenu", ui->menuMap.find("CustomMenu")->m_item.second.create);

		ui->menuMap.find(ConsoleRE::ContainerMenu::MENU_NAME)->m_item.second.create = ContainerMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::InventoryMenu::MENU_NAME)->m_item.second.create = InventoryMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::MagicMenu::MENU_NAME)->m_item.second.create = MagicMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::TweenMenu::MENU_NAME)->m_item.second.create = TweenMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::SleepWaitMenu::MENU_NAME)->m_item.second.create = SleepWaitMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::BarterMenu::MENU_NAME)->m_item.second.create = BarterMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::GiftMenu::MENU_NAME)->m_item.second.create = GiftMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::FavoritesMenu::MENU_NAME)->m_item.second.create = FavoritesMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::TrainingMenu::MENU_NAME)->m_item.second.create = TrainingMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::TutorialMenu::MENU_NAME)->m_item.second.create = TutorialMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::LockpickingMenu::MENU_NAME)->m_item.second.create = LockpickingMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::BookMenu::MENU_NAME)->m_item.second.create = BookMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::Console::MENU_NAME)->m_item.second.create = ConsoleEx::Creator;
		ui->menuMap.find(ConsoleRE::JournalMenu::MENU_NAME)->m_item.second.create = JournalMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::MessageBoxMenu::MENU_NAME)->m_item.second.create = MessageBoxMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::ModManagerMenu::MENU_NAME)->m_item.second.create = ModManagerMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::HUDMenu::MENU_NAME)->m_item.second.create = HUDMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::MapMenu::MENU_NAME)->m_item.second.create = MapMenuEx::Creator;;
		ui->menuMap.find(ConsoleRE::StatsMenu::MENU_NAME)->m_item.second.create = StatsMenuEx::Creator;
		ui->menuMap.find(ConsoleRE::LevelUpMenu::MENU_NAME)->m_item.second.create = LevelUpMenuEx::Creator;
		ui->menuMap.find("CustomMenu")->m_item.second.create = CustomMenuEx::Creator;

		ui->Register(CombatAlertOverlayMenu::MENU_NAME, CombatAlertOverlayMenu::Creator);

		ContainerMenuEx::ParseTranslations();
	}

	void InstallHooks()
	{	
		UnpausedTaskQueue::InstallHook();

		Papyrus::InstallHook();
		ConsoleCommandHooks::InstallHook();
		CameraMovement::InstallHook();
		Audio::InstallHook();
		ItemMenuUpdater::InstallHook();

		MenuControlsEx::InstallHook();
		PlayerControlsEx::InstallHook();

		BarterMenuEx::InstallHook();
		BookMenuEx::InstallHook();
		ConsoleEx::InstallHook();
		ContainerMenuEx::InstallHook();
		DialogueMenuEx::InstallHook();
		FavoritesMenuEx::InstallHook();
		GiftMenuEx::InstallHook();
		HUDMenuEx::InstallHook();
		InventoryMenuEx::InstallHook();
		JournalMenuEx::InstallHook();
		LevelUpMenuEx::InstallHook();
		LockpickingMenuEx::InstallHook();
		MagicMenuEx::InstallHook();
		MapMenuEx::InstallHook();
		MessageBoxMenuEx::InstallHook();
		ModManagerMenuEx::InstallHook();
		SleepWaitMenuEx::InstallHook();
		StatsMenuEx::InstallHook();
		TrainingMenuEx::InstallHook();
		TutorialMenuEx::InstallHook();
		TweenMenuEx::InstallHook();	
	}
}
