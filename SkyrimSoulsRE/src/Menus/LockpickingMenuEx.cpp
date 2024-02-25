#include "Menus/LockpickingMenuEx.h"

namespace SkyrimSoulsRE
{

	void LockpickingMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->UpdateHUD();
		}
		AutoCloseManager::GetSingleton()->CheckAutoClose(ConsoleRE::LockpickingMenu::MENU_NAME);
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	ConsoleRE::UI_MESSAGE_RESULTS LockpickingMenuEx::ProcessMessage_Hook(ConsoleRE::UIMessage& a_message)
	{
		if (a_message.type == ConsoleRE::UI_MESSAGE_TYPE::kHide)
		{
			HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::InterfaceStrings::GetSingleton()->hudMenu).get());
			if (hudMenu)
			{
				hudMenu->SetSkyrimSoulsMode(false);
			}
		}
		return _ProcessMessage(this, a_message);
	}

	ConsoleRE::IMenu* LockpickingMenuEx::Creator()
	{
		ConsoleRE::LockpickingMenu* menu = static_cast<ConsoleRE::LockpickingMenu*>(CreateMenu(ConsoleRE::LockpickingMenu::MENU_NAME));

		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
		autoCloseManager->InitAutoClose(ConsoleRE::LockpickingMenu::MENU_NAME, menu->GetTargetReference(), false);

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}

		return menu;
	}

	void LockpickingMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::LockpickingMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x5, &LockpickingMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x6, &LockpickingMenuEx::AdvanceMovie_Hook);

		REL::safe_write(Offsets::Menus::LockpickingMenu::Hook.address() + 0xED, uint16_t(0x47EB));
	}
}
